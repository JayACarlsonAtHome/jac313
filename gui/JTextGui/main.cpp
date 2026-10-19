// jText GUI — U++ window over jtext_gui_core.
// Open and save go through the canonical jText library. Database pull and
// mergesert go through IDbStore. Saved servers come from SQL_Secrets.

#include <CtrlLib/CtrlLib.h>

#include "jtext_gui/core/connections_file.hpp"
#include "jtext_gui/core/table_store.hpp"
#include "parser/section.hpp"

#include <fstream>
#include <functional>
#include <memory>

using namespace Upp;

String S(const std::string& s) { return String(s.data(), (int)s.size()); }
std::string Std(const String& s) { return s.ToStd(); }
String Field(const EditString& e) { return ~e; }

// Exclamation reads its text as QTF. A '[' in a server error, such as
// the '[' in a WHERE clause, is a format code and the dialog then says
// "Unexpected end of text" instead of the real message.
void Tell(const String& text) { Exclamation(DeQtfLf(text)); }

constexpr const char* kNullShown = "{NULL}";
constexpr const char* kEmptyShown = "{Empty String}";

String ShowField(const jtext_gui::Record& r, int i) {
	const auto n = (std::size_t)i;
	if (r.is_null[n]) return kNullShown;
	if (r.fields[n].empty()) return kEmptyShown;
	return S(r.fields[n]);
}

String ShowMark(const jtext_gui::Record& r, int i) {
	const auto n = (std::size_t)i;
	String mk;
	if (n < jtext_gui::kMaxFields && (r.has_line[n] || !r.field_markers[n].empty()))
		mk = S(jtext_gui::field_marker(r, n));
	else if (!r.marker.empty())
		mk = S(r.marker);
	return TrimBoth(mk).IsEmpty() ? String(jtext_gui::kDefaultMarker) : mk;
}

// '*' is a wildcard. A word with no star is a substring. Every word must hit.
bool FilterWord(const String& hay, const String& pat) {
	if (pat.Find('*') < 0)
		return hay.Find(pat) >= 0;
	int ti = 0, pi = 0, star = -1, mark = 0;
	while (ti < hay.GetCount()) {
		if (pi < pat.GetCount() && pat[pi] == '*') {
			star = pi++;
			mark = ti;
		} else if (pi < pat.GetCount() && pat[pi] == hay[ti]) {
			++ti;
			++pi;
		} else if (star >= 0) {
			pi = star + 1;
			ti = ++mark;
		} else
			return false;
	}
	while (pi < pat.GetCount() && pat[pi] == '*')
		++pi;
	return pi == pat.GetCount();
}

bool RowMatchesFilter(const jtext_gui::Record& r, const String& raw) {
	String filt = TrimBoth(raw);
	if (filt.IsEmpty())
		return true;
	String blob;
	for (std::size_t i = 0; i < jtext_gui::kMaxFields; ++i) {
		if (r.is_null[i])
			continue;
		if (!blob.IsEmpty())
			blob.Cat(' ');
		blob << S(r.fields[i]);
	}
	blob = ToLower(blob);
	Vector<String> words = Split(ToLower(filt), ' ');
	for (const String& w : words)
		if (!w.IsEmpty() && !FilterWord(blob, w))
			return false;
	return true;
}

struct ConnectDlg : TopWindow {
	typedef ConnectDlg CLASSNAME;

	DropList  saved, kind;
	EditString host, port, user, pass, database, file;
	Button    browse, saveFile, connectBtn, test, closeBtn;
	Label     status, lSaved, lKind, lHost, lPort, lUser, lPass, lDb, lFile;
	std::string path;
	std::vector<jtext_gui::SavedConnection> servers;
	bool filling = false;
	bool closing = false;
	// Called as soon as Connect succeeds, while the dialog is still open.
	std::function<void(const jtext_gui::ConnectionInfo&, std::unique_ptr<jtext_gui::IDbStore>)> on_linked;

	explicit ConnectDlg(const std::string& conn_path) : path(conn_path) {
		Title("Database connection");
		SetRect(0, 0, HorzLayoutZoom(740), VertLayoutZoom(400));
		Sizeable();
		kind.Add("SQLite");
		kind.Add("PostgreSQL");
		kind.Add("MariaDB");
		kind.SetIndex(0);
		pass.Password();
		lSaved.SetText("Saved");
		lKind.SetText("Database");
		lHost.SetText("Host");
		lPort.SetText("Port");
		lUser.SetText("User");
		lPass.SetText("Password");
		lDb.SetText("Database");
		lFile.SetText("SQLite file");
		browse.SetLabel("...");
		saveFile.SetLabel("Save to file");
		connectBtn.SetLabel("Connect");
		test.SetLabel("Test Connection");
		closeBtn.SetLabel("Close");
		int y = 8;
		Add(lSaved.LeftPosZ(12, 70).TopPosZ(y, 22));
		Add(saved.HSizePosZ(88, 12).TopPosZ(y, 24));
		y += 32;
		Add(lKind.LeftPosZ(12, 70).TopPosZ(y, 22));
		Add(kind.HSizePosZ(88, 12).TopPosZ(y, 24));
		y += 32;
		Add(lHost.LeftPosZ(12, 70).TopPosZ(y, 22));
		Add(host.HSizePosZ(88, 90).TopPosZ(y, 24));
		Add(lPort.RightPosZ(78, 36).TopPosZ(y, 22));
		Add(port.RightPosZ(12, 60).TopPosZ(y, 24));
		y += 32;
		Add(lUser.LeftPosZ(12, 70).TopPosZ(y, 22));
		Add(user.LeftPosZ(88, 160).TopPosZ(y, 24));
		Add(lPass.LeftPosZ(260, 70).TopPosZ(y, 22));
		Add(pass.HSizePosZ(336, 12).TopPosZ(y, 24));
		y += 32;
		Add(lDb.LeftPosZ(12, 70).TopPosZ(y, 22));
		Add(database.HSizePosZ(88, 12).TopPosZ(y, 24));
		y += 32;
		Add(lFile.LeftPosZ(12, 78).TopPosZ(y, 22));
		Add(file.HSizePosZ(96, 48).TopPosZ(y, 24));
		Add(browse.RightPosZ(12, 30).TopPosZ(y, 24));
		y += 36;
		Add(status.HSizePosZ(12, 12).TopPosZ(y, 44));
		Add(saveFile.LeftPosZ(12, 120).BottomPosZ(12, 28));
		Add(connectBtn.LeftPosZ(372, 100).BottomPosZ(12, 28));
		Add(test.LeftPosZ(480, 156).BottomPosZ(12, 28));
		Add(closeBtn.LeftPosZ(644, 84).BottomPosZ(12, 28));

		kind.WhenAction = THISBACK(ShowKind);
		saved.WhenAction = THISBACK(PickSaved);
		browse.WhenAction = THISBACK(BrowseFile);
		saveFile.WhenAction = THISBACK(SaveServer);
		test.WhenAction = THISBACK(TestConnection);
		connectBtn.WhenAction = THISBACK(OnConnect);
		closeBtn.WhenAction = THISBACK(OnClose);
		WhenClose = THISBACK(OnClose);
		RefreshSaved("");
		ShowKind();
	}

	void SetStatus(const String& text, bool bad) {
		status.SetText(text);
		status.SetInk(bad ? Color(170, 0, 0) : SColorText());
	}

	void ShowKind() {
		bool sqlite = kind.GetIndex() == 0;
		host.Enable(!sqlite);
		port.Enable(!sqlite);
		user.Enable(!sqlite);
		pass.Enable(!sqlite);
		database.Enable(true);
		file.Enable(sqlite);
		browse.Enable(sqlite);
		saveFile.Enable(!sqlite);
	}

	void RefreshSaved(const std::string& select_backend) {
		filling = true;
		saved.Clear();
		servers.clear();
		saved.Add("(choose a saved server)");
		if (path.empty()) {
			SetStatus("No connections file. Set $JTEXT_GUI_CONNECTIONS, outside git.", false);
			saved.SetIndex(0);
			filling = false;
			return;
		}
		std::string err;
		if (!jtext_gui::load_saved_servers(path, servers, err)) {
			servers.clear();
			SetStatus("No saved servers yet. Save to file creates " + S(path), false);
			saved.SetIndex(0);
			filling = false;
			return;
		}
		String borrowed;
		int pick = 0;
		for (int i = 0; i < (int)servers.size(); i++) {
			const auto& r = servers[i];
			if (!r.origin.empty() && borrowed.Find(S(r.origin)) < 0) {
				if (borrowed.GetCount()) borrowed << " and ";
				borrowed << S(r.origin);
			}
			String line = S(r.backend) + "  " + S(r.host);
			if (!r.dbname.empty()) line << "/" << S(r.dbname);
			if (!r.notes.empty()) line << "  (" << S(r.notes) << ")";
			if (!r.origin.empty()) line << "  [" << S(r.origin) << "]";
			saved.Add(line);
			if (!select_backend.empty() && r.backend == select_backend) pick = i + 1;
		}
		saved.SetIndex(pick);
		String git = S(jtext_gui::enclosing_git_project(path));
		String msg = Format("%d saved server(s) in %s", (int)servers.size(), S(path));
		if (git.GetCount()) msg << " — WARNING: inside the git project " << git << ", move it out";
		if (borrowed.GetCount()) msg << " Includes servers from " << borrowed << ".";
		SetStatus(msg, git.GetCount());
		filling = false;
	}

	void PickSaved() {
		if (filling) return;
		int i = saved.GetIndex() - 1;
		if (i < 0 || i >= (int)servers.size()) return;
		const auto& r = servers[i];
		std::string engine = r.backend.substr(0, r.backend.find('-'));
		if (engine == "pg") kind.SetIndex(1);
		else if (engine == "mariadb" || engine == "mysql") kind.SetIndex(2);
		host <<= S(r.host);
		port <<= S(r.port);
		user <<= S(r.user);
		database <<= S(r.dbname);
		pass <<= "";
		ShowKind();
	}

	void BrowseFile() {
		FileSel fs;
		fs.Type("SQLite", "*.db *.sqlite *.sqlite3");
		fs.AllFilesType();
		String cur = Field(file);
		if (DirectoryExists(GetFileFolder(cur)))
			fs.ActiveDir(GetFileFolder(cur));
		if (fs.ExecuteOpen("SQLite file"))
			file <<= ~fs;
	}

	void SaveServer() {
		if (kind.GetIndex() == 0) {
			SetStatus("SQLite is a file, not a server.", true);
			return;
		}
		if (path.empty()) {
			SetStatus("No connections file. Set $JTEXT_GUI_CONNECTIONS.", true);
			return;
		}
		jtext_gui::SavedConnection c;
		c.host = TrimBoth(Field(host)).ToStd();
		if (c.host.empty()) {
			SetStatus("Enter the host first.", true);
			return;
		}
		const char* engine = kind.GetIndex() == 1 ? "pg" : "mariadb";
		bool local = c.host == "localhost" || c.host == "127.0.0.1" || c.host == "::1";
		c.backend = std::string(engine) + (local ? "-local" : "-remote");
		c.port = TrimBoth(Field(port)).ToStd();
		c.user = TrimBoth(Field(user)).ToStd();
		c.dbname = TrimBoth(Field(database)).ToStd();
		std::vector<jtext_gui::SavedConnection> rows;
		std::string err;
		jtext_gui::load_connections(path, rows, err);
		jtext_gui::upsert_connection(rows, c);
		if (!jtext_gui::save_connections(path, rows, err)) {
			SetStatus("Not saved: " + S(err), true);
			return;
		}
		RefreshSaved(c.backend);
	}

	bool ReadInfo(jtext_gui::ConnectionInfo& info) {
		info = {};
		if (kind.GetIndex() == 0) {
			info.kind = jtext_gui::SessionKind::Sqlite;
			info.file_path = TrimBoth(Field(file)).ToStd();
			if (info.file_path.empty()) {
				Tell("Choose a SQLite file.");
				return false;
			}
		}
		else if (kind.GetIndex() == 1)
			info.kind = jtext_gui::SessionKind::PostgreSQL;
		else
			info.kind = jtext_gui::SessionKind::MariaDB;
		info.host = TrimBoth(Field(host)).ToStd();
		info.port = TrimBoth(Field(port)).ToStd();
		info.username = TrimBoth(Field(user)).ToStd();
		info.password = Std(Field(pass));
		info.database = TrimBoth(Field(database)).ToStd();
		if (info.kind != jtext_gui::SessionKind::Sqlite && info.host.empty()) {
			Tell("Enter the host.");
			return false;
		}
		return true;
	}

	void OnConnect() {
		jtext_gui::ConnectionInfo info;
		if (!ReadInfo(info)) return;
		auto made = jtext_gui::make_db_store(info.kind);
		if (!made || !made->connect(info)) {
			String why = made ? S(made->last_error()) : String();
			if (TrimBoth(why).IsEmpty()) why = "Could not connect.";
			SetStatus("Connect failed.", true);
			Tell(why);
			return;
		}
		if (on_linked)
			on_linked(info, std::move(made));
		else
			made->disconnect();
		SetStatus("Connected. The main window has this connection.", false);
		jtext_gui::debug_log(std::string("connection kept ") + jtext_gui::session_kind_name(info.kind));
	}

	void TestConnection() {
		jtext_gui::ConnectionInfo info;
		if (!ReadInfo(info)) return;
		auto made = jtext_gui::make_db_store(info.kind);
		if (!made || !made->connect(info)) {
			String why = made ? S(made->last_error()) : String();
			if (TrimBoth(why).IsEmpty()) why = "Could not connect.";
			SetStatus("Test failed.", true);
			Tell(why);
			return;
		}
		auto pong = made->ping();
		if (!pong) {
			made->disconnect();
			SetStatus("Test failed.", true);
			Tell(S(pong.error()));
			return;
		}
		if (on_linked)
			on_linked(info, std::move(made));
		else
			made->disconnect();
		SetStatus("SELECT 1 succeeded. The main window has this connection.", false);
		jtext_gui::debug_log(std::string("test kept ") + jtext_gui::session_kind_name(info.kind));
	}

	void OnClose() {
		if (closing) return;
		closing = true;
		Break(IDCANCEL);
	}

	void Run() { Execute(); }
};

struct TemplateDlg : TopWindow {
	typedef TemplateDlg CLASSNAME;

	DropList pick;
	DocEdit body;
	Button closeBtn;
	Vector<String> texts;

	TemplateDlg(const Vector<String>& names, const Vector<String>& bodies) {
		Title("Table and insert templates");
		SetRect(0, 0, 740, 520);
		Sizeable().Zoomable();
		for (int i = 0; i < names.GetCount(); ++i) {
			pick.Add(names[i]);
			texts.Add(bodies[i]);
		}
		body.SetReadOnly();
		body.SetFont(Monospace());
		closeBtn.SetLabel("Close");
		closeBtn.Cancel();
		closeBtn.WhenAction = [this] { Break(IDOK); };
		Add(pick.HSizePosZ(12, 12).TopPosZ(12, 24));
		Add(body.HSizePosZ(12, 12).VSizePosZ(48, 52));
		Add(closeBtn.RightPosZ(12, 80).BottomPosZ(12, 28));
		pick.WhenAction = [this] {
			int i = pick.GetIndex();
			body.Set(i >= 0 && i < texts.GetCount() ? texts[i] : String());
		};
		int prefer = 0;
		for (int i = 0; i < names.GetCount(); ++i) {
			String low = ToLower(names[i]);
			if (low.Find("create") >= 0) { prefer = i; break; }
		}
		if (pick.GetCount()) {
			pick.SetIndex(prefer);
			body.Set(texts[prefer]);
		}
	}
};

struct JTextGui : TopWindow {
	typedef JTextGui CLASSNAME;

	MenuBar menu;
	Label lTable, lWhere, lFilter, connStatus, status, lMarker, hint, num[10];
	EditString table, where, filter, marker, field[10], fieldMark[10];
	Vector<int> shown;
	Button connect, pull, openJt, saveJt, saveAs, mergesert, templatesBtn;
	Vector<String> templateNames, templateBodies;
	Splitter split;
	ArrayCtrl list;
	ParentCtrl fields;
	jtext_gui::TableSession session;
	jtext_gui::ConnectionInfo conn;
	std::unique_ptr<jtext_gui::IDbStore> store;
	bool loading = false;
	bool seen_dirty = false;
	bool closing = false;
	int loaded = -1;

	JTextGui() {
		Title("jText GUI");
		menu.Set([this](Bar& bar) {
			bar.Sub("File", [this](Bar& bar) {
				bar.Add("Open jText..", THISBACK(OpenJText)).Key(K_CTRL_O);
				bar.Add("Save jText", THISBACK(SaveJText)).Key(K_CTRL_S);
				bar.Add("Save jText As..", THISBACK(SaveJTextAs)).Key(K_SHIFT|K_CTRL_S);
				bar.Separator();
				bar.Add("Templates..", THISBACK(ShowTemplates));
				bar.Separator();
				bar.Add("Exit", [this] { Close(); });
			});
			bar.Sub("Help", [this](Bar& bar) {
				bar.Add("About", [] {
					PromptOK("jText GUI\n"
					         "Table editor for jText files and SQL tables.\n"
					         "U++ and C++23. Files go through the jText library.\n"
					         "Saved servers are the SQL_Secrets connection files.");
				});
			});
		});
		AddFrame(menu);

		connect.SetLabel("Connect...");
		pull.SetLabel("Pull");
		openJt.SetLabel("Open jText");
		saveJt.SetLabel("Save jText");
		saveAs.SetLabel("Save As");
		mergesert.SetLabel("Mergesert");
		templatesBtn.SetLabel("Templates");
		lTable.SetText("Table");
		lWhere.SetText("WHERE");
		lFilter.SetText("Filter");
		where.NullText("SQL for Pull, sent as WHERE");
		filter.NullText("search every field, e.g. hammer or *claw*");
		filter.WhenAction = THISBACK(OnFilter);
		connStatus.SetText("Not connected");
		lMarker.SetText("Marker");
		hint.SetText("default marker. Line 1 is the integer record id.");
		table <<= "my_table";
		Font mono = Monospace(StdFont().GetHeight());
		marker.SetFont(mono);
		marker <<= jtext_gui::kDefaultMarker;
		for (int i = 0; i < 10; i++) {
			num[i].SetText(i == 0 ? String("id") : Format("%2d.", i + 1));
			num[i].SetFont(mono);
			fieldMark[i].SetFont(mono);
			fieldMark[i].WhenAction = THISBACK(OnEdit);
			field[i].WhenAction = THISBACK(OnEdit);
		}
		// Tab moves through the ten data fields. Markers, the list, and the
		// pull row stay clickable and stay out of that order.
		table.NoWantFocus();
		where.NoWantFocus();
		filter.NoWantFocus();
		marker.NoWantFocus();
		list.NoWantFocus();
		connect.NoWantFocus();
		pull.NoWantFocus();
		openJt.NoWantFocus();
		saveJt.NoWantFocus();
		saveAs.NoWantFocus();
		mergesert.NoWantFocus();
		templatesBtn.NoWantFocus();
		for (int i = 0; i < 10; i++) {
			fieldMark[i].NoWantFocus();
			field[i].WantFocus();
		}

		int y = 8;
		Add(connect.LeftPosZ(8, 110).TopPosZ(y, 26));
		Add(lTable.LeftPosZ(128, 40).TopPosZ(y + 2, 22));
		Add(table.LeftPosZ(172, 180).TopPosZ(y, 26));
		Add(connStatus.HSizePosZ(364, 8).TopPosZ(y + 2, 22));
		y += 34;
		Add(lWhere.LeftPosZ(8, 50).TopPosZ(y + 2, 22));
		Add(where.HSizePosZ(64, 530).TopPosZ(y, 26));
		Add(pull.RightPosZ(438, 72).TopPosZ(y, 26));
		Add(openJt.RightPosZ(350, 82).TopPosZ(y, 26));
		Add(saveJt.RightPosZ(264, 80).TopPosZ(y, 26));
		Add(saveAs.RightPosZ(176, 82).TopPosZ(y, 26));
		Add(mergesert.RightPosZ(8, 90).TopPosZ(y, 26));
		y += 34;
		Add(lFilter.LeftPosZ(8, 50).TopPosZ(y + 2, 22));
		Add(filter.HSizePosZ(64, 8).TopPosZ(y, 26));
		y += 34;
		Add(split.HSizePosZ(8, 8).VSizePosZ(y, 28));
		Add(status.HSizePosZ(8, 8).BottomPosZ(4, 20));

		list.AddColumn("Records");
		list.WhenCursor = THISBACK(OnCursor);
		fields.Add(lMarker.LeftPosZ(8, 56).TopPosZ(8, 22));
		fields.Add(marker.LeftPosZ(68, 64).TopPosZ(6, 24));
		fields.Add(templatesBtn.RightPosZ(8, 110).TopPosZ(6, 24));
		fields.Add(hint.HSizePosZ(144, 126).TopPosZ(8, 22));
		for (int i = 0; i < 10; i++) {
			int yy = 40 + i * 30;
			fields.Add(num[i].LeftPosZ(8, 28).TopPosZ(yy, 22));
			fields.Add(fieldMark[i].LeftPosZ(40, 84).TopPosZ(yy - 2, 24));
			fields.Add(field[i].HSizePosZ(132, 8).TopPosZ(yy - 2, 24));
		}
		split.Set(list, fields);
		split.SetPos(2800);

		connect.WhenAction = THISBACK(Connect);
		pull.WhenAction = THISBACK(Pull);
		openJt.WhenAction = THISBACK(OpenJText);
		saveJt.WhenAction = THISBACK(SaveJText);
		saveAs.WhenAction = THISBACK(SaveJTextAs);
		mergesert.WhenAction = THISBACK(Mergesert);
		templatesBtn.WhenAction = THISBACK(ShowTemplates);

		Status("Open a jText file, or connect and pull a table.");
		const Vector<String>& args = CommandLine();
		for (int i = 0; i < args.GetCount(); i++) {
			if (args[i] == "--open" && i + 1 < args.GetCount())
				OpenPath(args[++i]);
			else if (!args[i].StartsWith("-"))
				OpenPath(args[i]);
		}
	}

	void Status(const String& text) { status.SetText(text); }

	void RefreshTitle() {
		String title = "jText GUI";
		String name;
		if (!session.jtext_path.empty())
			name = GetFileName(S(session.jtext_path));
		else if (session.is_open() && !session.table.empty())
			name = S(session.table);
		if (!name.IsEmpty())
			title << " - " << name;
		if (session.is_dirty())
			title << " *";
		Title(title);
	}

	// Status once, on the transition to dirty, so a later keystroke keeps a more specific note.
	void NoteEdit() {
		const bool dirty = session.is_dirty();
		RefreshTitle();
		if (dirty && !seen_dirty)
			Status("Unsaved changes. Save before closing.");
		seen_dirty = dirty;
	}

	void FinishLoad(const String& note) {
		ShowRows(note);
		RefreshTitle();
		seen_dirty = session.is_dirty();
	}

	void Close() override {
		if (closing) return;
		closing = true;
		SaveCurrent();
		if (session.is_dirty()) {
			const int answer = PromptSaveDontSaveCancel("Save changes before closing?");
			if (answer == 1) {
				if (jtext_gui::is_database_session(session.kind))
					Mergesert();
				else
					SaveJText();
				if (session.is_dirty()) {
					closing = false;
					return;
				}
			} else if (answer != 0) {
				closing = false;
				return;
			}
		}
		TopWindow::Close();
		closing = false;
	}

	String DefaultMarker() {
		String d = TrimBoth(Field(marker));
		if (d.IsEmpty())
			return String(jtext_gui::kDefaultMarker);
		return S(jtext_gui::live_marker(Std(d)));
	}

	void SaveCurrent() {
		if (loading || loaded < 0 || loaded >= (int)session.rows.size()) return;
		auto row = (std::size_t)loaded;
		String def = DefaultMarker();
		for (int i = 0; i < 10; i++) {
			if (i == 0) {
				ApplyRecordId(row, def);
				continue;
			}
			String raw = Field(field[i]);
			if (raw == kNullShown) {
				if (!session.rows[row].is_null[(std::size_t)i])
					session.set_null(row, (std::size_t)i);
				continue;
			}
			String mk = TrimBoth(Field(fieldMark[i]));
			if (mk.IsEmpty()) {
				const auto& rec = session.rows[row];
				if (rec.has_line[(std::size_t)i])
					mk = S(jtext_gui::field_marker(rec, (std::size_t)i));
				if (mk.IsEmpty())
					mk = def;
				loading = true;
				fieldMark[i] <<= mk;
				loading = false;
			}
			std::string value = raw == kEmptyShown ? std::string{} : Std(raw);
			session.set_field(row, (std::size_t)i, value);
			session.set_field_marker(row, (std::size_t)i, Std(mk));
		}
	}

	void LoadRow(int index) {
		if (index < 0 || index >= (int)session.rows.size()) return;
		loading = true;
		const auto& rec = session.rows[(std::size_t)index];
		if (TrimBoth(Field(marker)).IsEmpty())
			marker <<= jtext_gui::kDefaultMarker;
		String def = DefaultMarker();
		for (int i = 0; i < 10; i++) {
			field[i] <<= ShowField(rec, i);
			String mk = ShowMark(rec, i);
			if (TrimBoth(mk).IsEmpty())
				mk = def;
			fieldMark[i] <<= mk;
			if (i == 0)
				num[0].SetText("id");
			else if (i < (int)session.column_names.size())
				num[i].SetText(Format("%2d", i + 1));
		}
		loaded = index;
		loading = false;
	}

	void RefreshList() {
		int keep = loaded;
		int c = list.GetCursor();
		if (c >= 0 && c < shown.GetCount())
			keep = shown[c];
		String filt = Field(filter);
		loading = true;
		list.Clear();
		shown.Clear();
		int cursor = -1;
		for (int i = 0; i < (int)session.rows.size(); i++) {
			if (!RowMatchesFilter(session.rows[(std::size_t)i], filt))
				continue;
			if (i == keep)
				cursor = shown.GetCount();
			shown.Add(i);
			const auto& r = session.rows[(std::size_t)i];
			list.Add(Format("%d. %s %s", i + 1, S(r.marker), S(jtext_gui::record_preview(r))));
		}
		if (cursor < 0 && list.GetCount())
			cursor = 0;
		if (cursor >= 0)
			list.SetCursor(cursor);
		loading = false;
	}

	void OnFilter() {
		if (loading) return;
		SaveCurrent();
		RefreshList();
		if (shown.GetCount()) {
			int c = list.GetCursor();
			if (c < 0)
				c = 0;
			if (shown[c] != loaded)
				LoadRow(shown[c]);
		}
		String filt = TrimBoth(Field(filter));
		if (filt.IsEmpty())
			Status(Format("Showing all %d records.", (int)session.rows.size()));
		else
			Status(Format("%d of %d match \"%s\".", shown.GetCount(), (int)session.rows.size(), filt));
	}

	void OnEdit() {
		if (loading) return;
		SaveCurrent();
		RefreshList();
		NoteEdit();
	}

	void OnCursor() {
		if (loading) return;
		int c = list.GetCursor();
		if (c < 0 || c >= shown.GetCount()) return;
		int row = shown[c];
		if (row == loaded) return;
		SaveCurrent();
		LoadRow(row);
		NoteEdit();
	}

	void ShowRows(const String& note) {
		loaded = -1;
		RefreshList();
		if (shown.GetCount()) {
			loading = true;
			list.SetCursor(0);
			loading = false;
			LoadRow(shown[0]);
		}
		Status(note);
	}

	void OpenPath(const String& path) {
		SaveCurrent();
		auto r = jtext_gui::session_open_jtext(session, Std(path));
		if (!r) {
			Tell("Could not open jText:\n" + S(r.error()));
			return;
		}
		store.reset();
		connStatus.SetText("jText file");
		if (!session.table.empty())
			table <<= S(session.table);
		LoadTemplates(path);
		String note = "Opened " + path;
		if (templateNames.GetCount())
			note << ". " << templateNames.GetCount() << " templates — use Templates to read them.";
		if (session.is_dirty())
			note << ". Lines with no marker now use |#. Save before closing.";
		FinishLoad(note);
	}

	void ClearTemplates() {
		templateNames.Clear();
		templateBodies.Clear();
	}

	void LoadTemplates(const String& path) {
		ClearTemplates();
		std::ifstream in(Std(path));
		if (!in) return;
		auto parsed = jtext::parse_file_structure(in);
		if (!parsed) return;
		for (const auto& sec : parsed->sections) {
			for (const auto& t : sec.templates) {
				String label = S(t.name);
				if (!sec.name.empty())
					label = S(sec.name) + " — " + label;
				templateNames.Add(label);
				templateBodies.Add(S(t.body));
			}
		}
	}

	void ShowTemplates() {
		if (!templateNames.GetCount()) {
			Tell("This file has no table or insert templates.");
			return;
		}
		TemplateDlg(templateNames, templateBodies).Execute();
	}

	void OpenJText() {
		FileSel fs;
		fs.Type("jText files", "*.jtext *.jtFull");
		fs.AllFilesType();
		if (fs.ExecuteOpen("Open jText"))
			OpenPath(~fs);
	}

	bool ChooseSavePath(String& path, const char* title) {
		FileSel fs;
		fs.Type("jText files", "*.jtext");
		fs.AllFilesType();
		if (!path.IsEmpty())
			fs <<= path;
		if (!fs.ExecuteSaveAs(title)) return false;
		path = ~fs;
		return true;
	}

	void ApplyRecordId(std::size_t row, const String& def) {
		String raw = TrimBoth(Field(field[0]));
		std::string value = Std(raw);
		const bool ok = !raw.IsEmpty() && raw != kNullShown && raw != kEmptyShown &&
		                jtext_gui::parse_record_id(value).has_value();
		if (!ok) {
			if (!jtext_gui::record_id_of(session.rows[row]))
				jtext_gui::ensure_record_ids(session.rows);
			loading = true;
			field[0] <<= S(session.rows[row].fields[0]);
			if (TrimBoth(Field(fieldMark[0])).IsEmpty())
				fieldMark[0] <<= def;
			loading = false;
			Status("Line 1 is the record id. It has to be an integer.");
			return;
		}
		String mk = TrimBoth(Field(fieldMark[0]));
		if (mk.IsEmpty()) {
			mk = def;
			loading = true;
			fieldMark[0] <<= mk;
			loading = false;
		}
		session.set_field(row, 0, value);
		session.set_field_marker(row, 0, Std(mk));
	}

	bool RequireRecordIds() {
		auto fix = jtext_gui::ensure_record_ids(session.rows);
		if (fix.assigned) {
			RefreshList();
			if (loaded >= 0 && loaded < (int)session.rows.size())
				LoadRow(loaded);
			Status(Format("Assigned %d record id(s). Line 1 is the integer record id.", fix.assigned));
		}
		if (fix.not_integer) {
			Tell("Line 1 is the record id. Every record needs an integer there.");
			return false;
		}
		return true;
	}

	void WriteJText(const String& path, bool adopt) {
		if (!RequireRecordIds()) {
			RefreshTitle();
			seen_dirty = session.is_dirty();
			return;
		}
		std::string out = Std(path);
		bool as_file = session.kind == jtext_gui::SessionKind::JTextFile ||
		               session.kind == jtext_gui::SessionKind::None;
		if (as_file && adopt) {
			if (session.kind == jtext_gui::SessionKind::None) {
				session.kind = jtext_gui::SessionKind::JTextFile;
				session.jtext_path = out;
			}
			auto r = jtext_gui::session_save_jtext(session, out);
			if (!r) Tell("Could not save jText:\n" + S(r.error()));
			else Status("Saved " + path);
		} else {
			jtext_gui::TableShape shape;
			shape.table = Std(TrimBoth(Field(table)));
			if (shape.table.empty()) shape.table = session.table;
			shape.dialect = session.kind;
			shape.columns = session.columns;
			const jtext_gui::TableShape* use = shape.columns.empty() ? nullptr : &shape;
			auto r = jtext_gui::jtext_save(out, session.rows,
			                               shape.table.empty() ? "records" : shape.table, use);
			if (!r) Tell("Export failed:\n" + S(r.error()));
			else Status("Exported to " + path +
			            (use ? " with templates from the table." : ".") +
			            " The session stays " +
			            S(jtext_gui::session_kind_name(session.kind)) + ".");
		}
		RefreshTitle();
		seen_dirty = session.is_dirty();
	}

	void SaveJText() {
		SaveCurrent();
		String path = S(session.jtext_path);
		bool as_file = session.kind == jtext_gui::SessionKind::JTextFile ||
		               session.kind == jtext_gui::SessionKind::None;
		if (!as_file || path.IsEmpty()) {
			if (!ChooseSavePath(path, "Save jText")) return;
		}
		WriteJText(path, true);
	}

	void SaveJTextAs() {
		SaveCurrent();
		String path = S(session.jtext_path);
		if (!ChooseSavePath(path, "Save jText As")) return;
		bool as_file = session.kind == jtext_gui::SessionKind::JTextFile ||
		               session.kind == jtext_gui::SessionKind::None;
		WriteJText(path, as_file);
	}

	void Connect() {
		std::string where = jtext_gui::default_connections_path(Std(GetExeFilePath()));
		ConnectDlg dlg(where);
		dlg.on_linked = [this](const jtext_gui::ConnectionInfo& info,
		                       std::unique_ptr<jtext_gui::IDbStore> made) {
			conn = info;
			store = std::move(made);
			ClearTemplates();
			String label;
			if (conn.kind == jtext_gui::SessionKind::Sqlite)
				label = "SQLite: " + S(conn.file_path);
			else
				label = String(jtext_gui::session_kind_name(conn.kind)) + " " +
				        S(conn.username) + "@" + S(conn.host) + "/" + S(conn.database);
			connStatus.SetText(label);
			Status("Connected. Set the table and WHERE, then Pull.");
		};
		dlg.Run();
	}

	void Pull() {
		String tbl = TrimBoth(Field(table));
		if (tbl.IsEmpty()) {
			Tell("Enter a table name.");
			return;
		}
		if (!store || !store->is_connected()) {
			Tell("Connect to a database first.");
			return;
		}
		SaveCurrent();
		String wh = TrimBoth(Field(where));
		auto table_sql = jtext_gui::quote_table_ref(Std(tbl), store->kind());
		if (!table_sql) {
			Tell(S(table_sql.error()));
			return;
		}
		auto where_sql = jtext_gui::apply_ident_brackets(Std(wh), store->kind());
		if (!where_sql) {
			Tell(S(where_sql.error()));
			return;
		}
		String preview = "SELECT * FROM " + S(*table_sql);
		if (!where_sql->empty()) preview << " WHERE " << S(*where_sql);
		preview << " LIMIT 500";
		jtext_gui::debug_log(Std("pull " + preview));
		Status(preview);
		auto r = jtext_gui::session_open_db(session, *store, Std(tbl), Std(wh), 500);
		if (!r) {
			Status(connStatus.GetText() + " — pull failed");
			Tell("Pull failed. The connection is still open.\n" + connStatus.GetText() +
			     "\n\n" + S(r.error()) + "\n\nSent:\n" + preview);
			return;
		}
		ClearTemplates();
		FinishLoad(Format("Loaded %d rows from %s", (int)session.rows.size(), tbl));
	}

	void Mergesert() {
		SaveCurrent();
		if (session.kind == jtext_gui::SessionKind::JTextFile) {
			Tell("This session is a jText file. Use Save jText.");
			return;
		}
		if (!store || !store->is_connected() || !jtext_gui::is_database_session(session.kind)) {
			Tell("Connect and pull a table first. Mergesert writes dirty rows only.");
			return;
		}
		auto r = jtext_gui::session_save_db(session, *store);
		if (!r) Tell("Mergesert failed:\n" + S(r.error()));
		else Status(Format("Wrote %d rows.", (int)*r));
		RefreshTitle();
		seen_dirty = session.is_dirty();
	}
};

GUI_APP_MAIN
{
	JTextGui().Run();
}
