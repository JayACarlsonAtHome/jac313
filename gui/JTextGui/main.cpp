// jText GUI — U++ window over jtext_gui_core.
// Open and save go through the canonical jText library. Database pull and
// mergesert go through IDbStore. Saved servers come from SQL_Secrets.

#include <CtrlLib/CtrlLib.h>

#include "jtext_gui/core/connections_file.hpp"
#include "jtext_gui/core/table_store.hpp"
#include "jtext_gui/core/spreadsheet.hpp"
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

// How a field shows a NULL (no line) and an empty string (a line with no text).
constexpr const char* kNullShown = "{Null}";
constexpr const char* kEmptyShown = "{No Value or Empty String}";

String ShowField(const jtext_gui::Record& r, int i) {
	const auto n = (std::size_t)i;
	if (jtext_gui::field_is_null(r, n)) return kNullShown;
	if (jtext_gui::field_text(r, n).empty()) return kEmptyShown;
	return S(r.fields[n]);
}

String ShowMark(const jtext_gui::Record& r, int i) {
	const auto n = (std::size_t)i;
	String mk;
	if (n < r.size() && (r.has_line[n] || !r.field_markers[n].empty()))
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

// Like a database: the session's Case decides whether the Filter ignores letter case.
bool RowMatchesFilter(const jtext_gui::Record& r, const String& raw, bool case_sensitive) {
	String filt = TrimBoth(raw);
	if (filt.IsEmpty())
		return true;
	String blob;
	for (std::size_t i = 0; i < r.size(); ++i) {
		if (r.is_null[i])
			continue;
		if (!blob.IsEmpty())
			blob.Cat(' ');
		blob << S(r.fields[i]);
	}
	if (!case_sensitive) {
		blob = ToLower(blob);
		filt = ToLower(filt);
	}
	Vector<String> words = Split(filt, ' ');
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

// A record's lines: as many rows as it needs (up to 99), with a scroll bar on the right.
// The rows live in `content`, which is moved up as the bar scrolls. The mouse wheel
// scrolls it, and Tab into a row out of sight scrolls that row into view.
struct LinesView : ParentCtrl {
	typedef LinesView CLASSNAME;

	ScrollBar  sb;
	ParentCtrl content;
	int        contentHeight = 0;
	Event<Ctrl&> WhenRowFocus;   // a control in one of the rows got the focus

	LinesView() {
		AddFrame(sb.Vert());
		sb.WhenScroll = [this] { Place(); };
		Add(content);
	}

	void SetContentHeight(int cy, int line) {
		contentHeight = cy;
		sb.SetLine(line);
		Layout();
	}

	void Layout() override {
		sb.Set(sb.Get(), GetSize().cy, contentHeight);
		Place();
	}

	void Place() {
		const Size sz = GetSize();
		content.SetRect(0, -sb.Get(), sz.cx, max(contentHeight, sz.cy));
	}

	void MouseWheel(Point, int zdelta, dword) override { sb.Wheel(zdelta); }

	void ChildGotFocus() override {
		Ctrl *f = GetFocusCtrl();
		if (f && f->GetParent() == &content) {
			const Rect r = f->GetRect();
			sb.ScrollInto(max(r.top - 4, 0), r.Height() + 8);
			WhenRowFocus(*f);
		}
		ParentCtrl::ChildGotFocus();
	}
};

// Where things were left, restored at the next start: the main window, the splitter, and
// whether the Records list was in its own window (and that window's place).
// Saved as JSON in U++'s config folder (JTextGui.json).
struct GuiSettings {
	int x = 0, y = 0, cx = 0, cy = 0;
	bool maximized = false;
	int split = 4000;
	bool detached = false;
	int list_x = 0, list_y = 0, list_cx = 0, list_cy = 0;

	void Jsonize(JsonIO& io) {
		io("x", x)("y", y)("cx", cx)("cy", cy)("maximized", maximized)("split", split)
		  ("detached", detached)("list_x", list_x)("list_y", list_y)
		  ("list_cx", list_cx)("list_cy", list_cy);
	}

	// A saved rectangle, when it is still big enough and on a screen.
	static bool Usable(const Rect& r, Size min) {
		return r.Width() >= min.cx && r.Height() >= min.cy && Ctrl::GetVirtualWorkArea().Intersects(r);
	}
};

// The Records list in a window of its own (Detach), e.g. on a second monitor. It is opened
// with OpenMain (no owner), so it is a normal window: it moves freely and maximizes like
// any other. Closing it puts the list back (WhenClose).
struct RecordsWindow : TopWindow {
	Event<> WhenClose;
	void Close() override { WhenClose(); }
	void CloseNow() { TopWindow::Close(); }
};

// A line's value field: Ctrl+N (any case) asks for NULL.
struct FieldEdit : EditString {
	Event<> WhenNull;
	bool Key(dword key, int rep) override {
		if (key == (K_CTRL|K_N) || key == (K_SHIFT|K_CTRL|K_N)) {
			WhenNull();
			return true;
		}
		return EditString::Key(key, rep);
	}
};

// A spreadsheet-style grid over the records: one row per record, one column per chosen
// line. Only the visible cells are drawn. Two modes, Excel style:
//   Cell mode (the grid has the focus, the current cell is highlighted = its whole value
//   selected): arrows/Enter/Tab/PageUp/PageDown move; typing starts editing with the typed
//   text replacing the value; Delete/Backspace empties it; Ctrl+C copies the value; Ctrl+V
//   pastes one value over it; Ctrl+N (any case) makes it NULL; Ctrl+Z undoes the last saved
//   cell change.
//   Edit mode (an editor on the cell; F2, Home, End, typing, or a click on the current
//   cell): Left/Right move letter by letter and, at the text's start or end, save and go to
//   the next cell; Up/Down/Enter/Tab save and move; Esc drops the typing; Ctrl+Z drops the
//   typing (else undoes the last saved change); Ctrl+N saves NULL.
// Paste takes one value: text with tabs or line breaks (a row, a column or a block) is
// refused. A locked cell (the record id, until unlocked) can only be copied.
// Cell mode selects nothing in a text field on purpose: a text selection becomes the
// desktop's primary selection, and doing that on every move floods a Wayland desktop.
struct CellGrid : Ctrl {
	typedef CellGrid CLASSNAME;

	enum Kind { VALUE, EMPTY, NUL };
	struct Cell { String text; Kind kind = VALUE; };

	// Supplied by the window.
	Function<int ()>                    GetRows;
	Function<int ()>                    GetCols;
	Function<String (int c)>            GetTitle;
	Function<int (int c)>               GetWidthChars;
	Function<String (int r)>            GetRowTitle;
	Function<Cell (int r, int c)>       GetCell;
	Function<bool (int r, int c)>       IsLocked;
	Function<bool (int r, int c, const String& text, bool null)> Commit;   // false: refused
	Function<bool ()>                   UndoLast;   // false: nothing to undo
	Event<int>                          WhenRow;    // the current row changed
	Event<const String&>                WhenStatus;

	int  cr = 0, cc = 0;      // the current cell
	bool editing = false;     // edit mode: the editor is on the cell
	String orig;              // the editor's text when editing began

	struct Editor : EditString {
		CellGrid *g = nullptr;
		int caretAfterFocus = -1;   // EditField selects all on focus; edit mode wants a caret
		void GotFocus() override {
			EditString::GotFocus();
			if (caretAfterFocus >= 0) {
				SetSelection(caretAfterFocus, caretAfterFocus);
				caretAfterFocus = -1;
			}
		}
		bool Key(dword key, int rep) override {
			if (g && g->EditKey(key)) return true;
			return EditString::Key(key, rep);
		}
		void MouseWheel(Point p, int zdelta, dword keyflags) override {
			if (g) g->MouseWheel(p, zdelta, keyflags);
		}
		void LostFocus() override {
			EditString::LostFocus();
			if (g && g->editing) g->EndEdit(true, false);   // e.g. a click on the right side
		}
	};
	Editor ed;
	ScrollBars sb;
	Font font;

	CellGrid() {
		font = Monospace(StdFont().GetHeight());
		ed.g = this;
		ed.SetFont(font);
		ed.Hide();
		AddFrame(sb);
		sb.WhenScroll = [this] { PlaceEditor(); Refresh(); };
		Add(ed);
		WantFocus();
	}

	static String Shown(const Cell& c) {   // the text a cell shows
		return c.kind == NUL ? String(kNullShown) : c.kind == EMPTY ? String(kEmptyShown) : c.text;
	}

	int CharCx() const   { return GetTextSize("0", font).cx; }
	int RowCy() const    { return font.GetCy() + Zy(8); }
	int HeadCy() const   { return RowCy(); }
	int Rows() const     { return GetRows ? GetRows() : 0; }
	int Cols() const     { return GetCols ? GetCols() : 0; }
	int RowHeadCx() const {
		int n = 2;
		for (int r = 0; r < Rows(); r++) n = max(n, GetRowTitle(r).GetCount());
		return CharCx() * n + Zx(12);
	}
	int ColCx(int c) const {
		const int chars = max(GetWidthChars(c), GetTitle(c).ToWString().GetCount());
		return CharCx() * chars + Zx(14);
	}
	int ColX(int c) const {   // left edge, unscrolled
		int x = RowHeadCx();
		for (int i = 0; i < c; i++) x += ColCx(i);
		return x;
	}
	int TotalCx() const { return ColX(Cols()); }
	bool Valid() const  { return Rows() > 0 && Cols() > 0; }

	// The data or the columns changed: keep the current cell where possible.
	void Reset() {
		if (editing) EndEdit(false);
		cr = clamp(cr, 0, max(Rows() - 1, 0));
		cc = clamp(cc, 0, max(Cols() - 1, 0));
		Layout();
		Refresh();
	}

	void Layout() override {
		const Size sz = GetSize();
		sb.SetPage(Size(sz.cx, sz.cy - HeadCy()));
		sb.SetTotal(Size(TotalCx(), Rows() * RowCy()));
		sb.SetLine(CharCx() * 4, RowCy());
		PlaceEditor();
	}

	Rect CellRect(int r, int c) const {
		const Point o = sb.Get();
		return RectC(ColX(c) - o.x, HeadCy() + r * RowCy() - o.y, ColCx(c), RowCy());
	}

	void PlaceEditor() {
		if (editing) ed.SetRect(CellRect(cr, cc).Deflated(1));
	}

	void EnsureVisible() {
		const Rect r = CellRect(cr, cc);
		const Point o = sb.Get();
		sb.ScrollInto(Rect(r.left + o.x - RowHeadCx(), r.top + o.y - HeadCy(),
		                   r.right + o.x - RowHeadCx(), r.bottom + o.y - HeadCy()));
	}

	bool LockedHere() {
		if (!IsLocked(cr, cc)) return false;
		WhenStatus("The record id is locked (Unlock Row ID to change it).");
		return true;
	}

	// Edit mode on the current cell, starting from text with the caret at caret (-1: end).
	void BeginEdit(const String& text, int caret = -1) {
		if (!Valid() || LockedHere()) return;
		const Cell cell = GetCell(cr, cc);
		orig = Shown(cell);
		editing = true;
		ed <<= text;
		const int at = caret < 0 ? ed.GetLength() : min(caret, ed.GetLength());
		ed.caretAfterFocus = at;
		PlaceEditor();
		ed.Show();
		ed.SetFocus();
		ed.SetSelection(at, at);   // also when the editor had the focus already
		ed.caretAfterFocus = -1;
	}

	// Leave edit mode, saving the text when it changed (accept) or dropping it.
	// False: the save was refused (edit mode stays).
	bool EndEdit(bool accept, bool refocus = true) {
		if (!editing) return true;
		const String now = ~ed;
		if (accept && now != orig) {
			if (!Commit(cr, cc, now, false)) {
				ed.SetFocus();
				return false;
			}
		}
		editing = false;
		ed.Hide();
		if (refocus) SetFocus();
		Refresh();
		return true;
	}

	void MoveTo(int r, int c) {
		if (!Valid()) return;
		if (editing && !EndEdit(true)) return;
		r = clamp(r, 0, Rows() - 1);
		c = clamp(c, 0, Cols() - 1);
		const bool rowChanged = r != cr;
		cr = r;
		cc = c;
		if (rowChanged) WhenRow(cr);
		EnsureVisible();
		Refresh();
		if (!HasFocus()) SetFocus();
	}

	// One value from the clipboard, or refused (a row, a column or a block).
	bool ClipboardValue(String& t) {
		t = ReadClipboardUnicodeText().ToString();
		while (t.EndsWith("\n") || t.EndsWith("\r")) t.Trim(t.GetCount() - 1);   // Excel's one cell
		if (t.Find('\t') >= 0 || t.Find('\n') >= 0 || t.Find('\r') >= 0) {
			WhenStatus("Paste one value at a time: that clipboard holds a row, a column or a block.");
			return false;
		}
		return true;
	}

	int PageRows() const { return max(GetSize().cy / RowCy() - 2, 1); }

	// Cell mode keys (the grid has the focus).
	bool Key(dword key, int) override {
		if (!Valid()) return false;
		switch (key) {
		case K_UP:        MoveTo(cr - 1, cc); return true;
		case K_DOWN:
		case K_ENTER:     MoveTo(cr + 1, cc); return true;
		case K_LEFT:
		case K_SHIFT_TAB: MoveTo(cr, cc - 1); return true;
		case K_RIGHT:
		case K_TAB:       MoveTo(cr, cc + 1); return true;
		case K_PAGEUP:    MoveTo(cr - PageRows(), cc); return true;
		case K_PAGEDOWN:  MoveTo(cr + PageRows(), cc); return true;
		case K_CTRL_HOME: MoveTo(0, cc); return true;
		case K_CTRL_END:  MoveTo(Rows() - 1, cc); return true;
		case K_F2:
		case K_HOME:
		case K_END: {
			const Cell cell = GetCell(cr, cc);
			BeginEdit(cell.kind == VALUE ? cell.text : String(), key == K_HOME ? 0 : -1);
			return true;
		}
		case K_DELETE:
		case K_BACKSPACE:
			if (!LockedHere()) Commit(cr, cc, String(), false);
			Refresh();
			return true;
		case K_CTRL|K_C:
		case K_CTRL|K_INSERT: {
			const Cell cell = GetCell(cr, cc);
			WriteClipboardText(cell.kind == VALUE ? cell.text : String());
			WhenStatus("Copied the cell.");
			return true;
		}
		case K_CTRL|K_V:
		case K_SHIFT|K_INSERT: {
			String t;
			if (!LockedHere() && ClipboardValue(t)) Commit(cr, cc, t, false);
			Refresh();
			return true;
		}
		case K_CTRL|K_N:
		case K_SHIFT|K_CTRL|K_N:
			if (!LockedHere()) Commit(cr, cc, String(), true);
			Refresh();
			return true;
		case K_CTRL|K_Z:
			if (!(UndoLast && UndoLast())) WhenStatus("Nothing to undo.");
			EnsureVisible();
			Refresh();
			return true;
		}
		if (key < K_CHAR_LIM && key >= 32) {   // typing replaces the value
			if (LockedHere()) return true;
			BeginEdit(String());
			if (editing) ed.Insert((int)key);
			return true;
		}
		return false;
	}

	// Edit mode keys (the editor has the focus). True: handled here.
	bool EditKey(dword key) {
		int l, h;
		ed.GetSelection(l, h);
		const bool atStart = l == 0 && h == 0;
		const bool atEnd = l == ed.GetLength() && h == l;
		switch (key) {
		case K_UP:        MoveTo(cr - 1, cc); return true;
		case K_DOWN:
		case K_ENTER:     MoveTo(cr + 1, cc); return true;
		case K_TAB:       MoveTo(cr, cc + 1); return true;
		case K_SHIFT_TAB: MoveTo(cr, cc - 1); return true;
		case K_LEFT:
			if (atStart) { MoveTo(cr, cc - 1); return true; }
			return false;
		case K_RIGHT:
			if (atEnd) { MoveTo(cr, cc + 1); return true; }
			return false;
		case K_ESCAPE:
			EndEdit(false);
			return true;
		case K_CTRL|K_N:
		case K_SHIFT|K_CTRL|K_N:
			EndEdit(false);
			Commit(cr, cc, String(), true);
			Refresh();
			return true;
		case K_CTRL|K_Z: {
			const Cell cell = GetCell(cr, cc);
			const String start = cell.kind == VALUE ? cell.text : String();
			if (~ed != start) {   // drop the typing first
				ed <<= start;
				ed.SetSelection(ed.GetLength(), ed.GetLength());
				return true;
			}
			EndEdit(false);
			if (!(UndoLast && UndoLast())) WhenStatus("Nothing to undo.");
			EnsureVisible();
			Refresh();
			return true;
		}
		case K_CTRL|K_V:
		case K_SHIFT|K_INSERT: {
			String t;
			if (ClipboardValue(t)) ed.Insert(t.ToWString());
			return true;
		}
		}
		return false;
	}

	void LeftDown(Point p, dword) override {
		const Point o = sb.Get();
		if (p.y < HeadCy() || p.x < RowHeadCx()) return;
		const int r = (p.y - HeadCy() + o.y) / RowCy();
		int c = 0, x = RowHeadCx() - o.x;
		while (c < Cols() && x + ColCx(c) <= p.x) x += ColCx(c++);
		if (r >= Rows() || c >= Cols()) return;
		if (r == cr && c == cc && !editing) {   // a click on the current cell: edit its text
			const Cell cell = GetCell(cr, cc);
			BeginEdit(cell.kind == VALUE ? cell.text : String());
			return;
		}
		MoveTo(r, c);
	}

	void LeftDouble(Point p, dword flags) override { LeftDown(p, flags); }

	void MouseWheel(Point, int zdelta, dword) override { sb.WheelY(zdelta); }

	void GotFocus() override { Refresh(); }
	void LostFocus() override { Refresh(); }

	void Paint(Draw& w) override {
		const Size sz = GetSize();
		w.DrawRect(sz, SColorPaper());
		const Point o = sb.Get();
		const int rowCy = RowCy(), headCy = HeadCy(), rhCx = RowHeadCx();
		const int first = max(o.y / rowCy, 0);
		const int last = min(Rows(), (o.y + sz.cy) / rowCy + 1);
		const Color line = SColorShadow(), face = SColorFace();
		const bool focused = HasFocus() || editing;
		for (int r = first; r < last; r++) {
			const int y = headCy + r * rowCy - o.y;
			for (int c = 0; c < Cols(); c++) {
				const int x = ColX(c) - o.x;
				const int cx = ColCx(c);
				if (x > sz.cx || x + cx < rhCx) continue;
				const Cell cell = GetCell(r, c);
				const bool current = r == cr && c == cc;
				Color paper = r == cr ? Blend(SColorPaper(), SColorHighlight(), 30) : SColorPaper();
				if (IsLocked(r, c)) paper = Blend(paper, face, 128);
				Color ink = cell.kind == VALUE ? SColorText() : SColorDisabled();
				if (current && focused && !editing) {   // the whole value selected
					paper = SColorHighlight();
					ink = SColorHighlightText();
				}
				w.DrawRect(x, y, cx, rowCy, paper);
				w.Clip(x + 1, y, cx - 2, rowCy);
				w.DrawText(x + Zx(5), y + Zy(4), Shown(cell), font, ink);
				w.End();
				w.DrawRect(x + cx - 1, y, 1, rowCy, line);
				w.DrawRect(x, y + rowCy - 1, cx, 1, line);
			}
			w.DrawRect(0, y, rhCx, rowCy, r == cr ? Blend(face, SColorHighlight(), 60) : face);
			w.DrawText(Zx(4), y + Zy(4), GetRowTitle(r), font, SColorText());
			w.DrawRect(0, y + rowCy - 1, rhCx, 1, line);
		}
		w.DrawRect(0, 0, sz.cx, headCy, face);
		for (int c = 0; c < Cols(); c++) {
			const int x = ColX(c) - o.x;
			const int cx = ColCx(c);
			if (x > sz.cx || x + cx < rhCx) continue;
			w.Clip(max(x, rhCx), 0, cx, headCy);
			w.DrawText(x + Zx(5), Zy(4), GetTitle(c), font().Bold(), SColorText());
			w.End();
			w.DrawRect(x + cx - 1, 0, 1, headCy, line);
		}
		w.DrawRect(0, 0, rhCx, headCy, face);
		w.DrawText(Zx(4), Zy(4), "#", font().Bold(), SColorText());
		w.DrawRect(0, headCy - 1, sz.cx, 1, line);
		w.DrawRect(rhCx - 1, 0, 1, sz.cy, line);
		if (Valid()) {   // the current cell's frame
			const Rect cur = CellRect(cr, cc);
			const Color hl = SColorHighlight();
			w.DrawRect(cur.left, cur.top, cur.Width(), 2, hl);
			w.DrawRect(cur.left, cur.bottom - 2, cur.Width(), 2, hl);
			w.DrawRect(cur.left, cur.top, 2, cur.Height(), hl);
			w.DrawRect(cur.right - 2, cur.top, 2, cur.Height(), hl);
		}
	}
};

struct JTextGui : TopWindow {
	typedef JTextGui CLASSNAME;

	MenuBar menu;
	Label lTable, lWhere, lFilter, connStatus, status, lMarker, hint;
	EditString table, where, filter, marker;
	// A jText file's sections (dev, test, prod, ...): pick the one to edit. Shown when the
	// file has more than one.
	Label lSection;
	DropList section;
	bool fillingSections = false;
	// One row per line of the record: number, marker, value, the line's comment. Created on
	// demand (at least kDefaultFields, at most kMaxFields); rows past `visible` are hidden.
	// The comment is its text alone: the comment character is the marker's, written only
	// in the file.
	Array<Label> num;
	Array<FieldEdit> field;
	Array<EditString> fieldMark;
	Array<EditString> fieldComment;
	LinesView lines;
	int visible = 0;
	Vector<int> lineOf;      // row -> field index (0 is the id); all lines, or the Lines list
	int focusRow = 0;        // the row the field box below describes
	Button addLine;
	Label lLines;
	EditString lineList;     // "24, 13, 45" or "5-9": only these lines (after the id)
	EditString fieldInfo;    // read-only: the line being edited, its field and value
	Vector<int> shown;
	Button connect, pull, openJt, saveJt, saveAs, mergesert, templatesBtn;
	Vector<String> templateNames, templateBodies;
	Splitter split;
	ArrayCtrl list;
	ParentCtrl listPane;      // the Records list and its Detach/Attach button
	Button detachBtn;
	Label lRecords;
	CellGrid grid;            // the Records as a grid, while a Lines list is active
	Vector<int> gridCols;     // grid column -> field (0 = the id, then the Lines list)
	Vector<int> gridWidths;   // characters
	struct UndoStep { int row; int field; bool was_null; bool had_line; std::string text; };
	Array<UndoStep> undo;     // grid cell changes, newest last (Ctrl+Z)
	bool idLocked = true;     // the record id can be changed only after Unlock Row ID
	Button unlockId;
	RecordsWindow recordsWin;
	bool detached = false;
	int splitPos = 4000;      // the splitter's place while the list is detached
	GuiSettings settings;
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
		// Resizable, and opened at 90% of the monitor it starts on (its work area, so not
		// under a panel), centred there; the left and right sides grow with it.
		Sizeable().Zoomable();
		const Size minSize(Zx(900), Zy(560));
		SetMinSize(minSize);
		LoadFromJsonFile(settings);
		const Rect saved(settings.x, settings.y, settings.x + settings.cx, settings.y + settings.cy);
		if (GuiSettings::Usable(saved, minSize))
			SetRect(saved);
		else {
			const Rect work = GetWorkArea(GetMousePos());
			const Size want(work.Width() * 9 / 10, work.Height() * 9 / 10);
			SetRect(work.CenterRect(max(want.cx, minSize.cx), max(want.cy, minSize.cy)));
		}
		if (settings.maximized)
			Maximize();
		menu.Set([this](Bar& bar) {
			bar.Sub("File", [this](Bar& bar) {
				bar.Add("Open jText..", THISBACK(OpenJText)).Key(K_CTRL_O);
				bar.Add("Save jText", THISBACK(SaveJText)).Key(K_CTRL_S);
				bar.Add("Save jText As..", THISBACK(SaveJTextAs)).Key(K_SHIFT|K_CTRL_S);
				bar.Separator();
				bar.Add("Export Excel (.xlsx)..", THISBACK(ExportXlsx));
				bar.Add("Import Excel (.xlsx)..", THISBACK(ImportXlsx));
				bar.Separator();
				bar.Add("Templates..", THISBACK(ShowTemplates));
				bar.Separator();
				bar.Add("Exit", [this] { Close(); });
			});
			bar.Sub("Help", [](Bar& bar) {
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
		// Tab moves through the data fields. Markers, the list, and the
		// pull row stay clickable and stay out of that order.
		table.NoWantFocus();
		section.NoWantFocus();
		lSection.SetText("Section");
		section.WhenAction = THISBACK(OnSection);
		where.NoWantFocus();
		// the Filter box takes the focus at start (ActiveFocus below), so it must want focus
		marker.NoWantFocus();
		list.NoWantFocus();
		connect.NoWantFocus();
		pull.NoWantFocus();
		openJt.NoWantFocus();
		saveJt.NoWantFocus();
		saveAs.NoWantFocus();
		mergesert.NoWantFocus();
		templatesBtn.NoWantFocus();
		addLine.NoWantFocus();

		int y = 8;
		Add(connect.LeftPosZ(8, 110).TopPosZ(y, 26));
		Add(lTable.LeftPosZ(128, 40).TopPosZ(y + 2, 22));
		Add(table.LeftPosZ(172, 180).TopPosZ(y, 26));
		Add(lSection.LeftPosZ(364, 56).TopPosZ(y + 2, 22));
		Add(section.LeftPosZ(424, 200).TopPosZ(y, 26));
		Add(connStatus.HSizePosZ(636, 8).TopPosZ(y + 2, 22));
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

		list.AddColumn("Records", Zx(260));
		list.HeaderObject().Absolute();   // the column is as wide as its rows; scroll sideways
		list.WhenCursor = THISBACK(OnCursor);
		fields.Add(lMarker.LeftPosZ(8, 56).TopPosZ(8, 22));
		fields.Add(marker.LeftPosZ(68, 64).TopPosZ(6, 24));
		fields.Add(templatesBtn.RightPosZ(8, 110).TopPosZ(6, 24));
		fields.Add(addLine.RightPosZ(126, 70).TopPosZ(6, 24));
		fields.Add(unlockId.RightPosZ(204, 120).TopPosZ(6, 24));
		fields.Add(hint.HSizePosZ(144, 332).TopPosZ(8, 22));
		unlockId.SetLabel("Unlock Row ID");
		unlockId.Tip("The record id (line 1) is locked so it is not changed by accident");
		unlockId.NoWantFocus();
		unlockId.WhenAction = THISBACK(ToggleIdLock);
		addLine.SetLabel("+ Line");
		addLine.Tip(Format("Add a line to this record (up to %d)", (int)jtext_gui::kMaxFields));
		addLine.WhenAction = THISBACK(AddLine);
		lLines.SetText("Lines");
		lineList.NullText("all lines; or e.g. 24, 13, price, 5-9 (shown in this order, after the id)");
		lineList.WhenAction = THISBACK(OnLineList);
		lineList.NoWantFocus();
		fields.Add(lLines.LeftPosZ(8, 56).TopPosZ(38, 22));
		fields.Add(lineList.HSizePosZ(68, 8).TopPosZ(36, 24));
		fieldInfo.SetReadOnly();
		fieldInfo.NoWantFocus();
		fieldInfo.SetFont(mono);
		fields.Add(lines.HSizePosZ(0, 0).VSizePosZ(66, 34));
		fields.Add(fieldInfo.HSizePosZ(8, 8).BottomPosZ(6, 24));
		lines.WhenRowFocus = THISBACK(OnRowFocus);
		for (int i = 0; i < (int)jtext_gui::kDefaultFields; i++)
			lineOf.Add(i);
		SetVisibleRows(lineOf.GetCount());
		// The Records list with a Detach button over the right end of its header.
		detachBtn.SetLabel("Detach");
		detachBtn.Tip("Move the Records list into a window of its own (e.g. on another monitor)");
		detachBtn.NoWantFocus();
		detachBtn.WhenAction = [this] { if (detached) Attach(); else Detach(); };
		// The Records panel: a title row with Detach, then the plain list or (with a
		// Lines list) the grid.
		lRecords.SetText("Records");
		listPane.Add(lRecords.LeftPosZ(4, 200).TopPosZ(2, 22));
		listPane.Add(detachBtn.RightPosZ(2, 70).TopPosZ(1, 22));
		listPane.Add(list.HSizePosZ(0, 0).VSizePosZ(26, 0));
		listPane.Add(grid.HSizePosZ(0, 0).VSizePosZ(26, 0));
		grid.Hide();
		SetupGrid();
		recordsWin.Title("jText GUI - Records").Sizeable().Zoomable();
		recordsWin.WhenClose = [this] { Attach(); };
		splitPos = settings.split > 0 && settings.split < 10000 ? settings.split : 4000;
		split.Set(listPane, fields);
		split.SetPos(splitPos);   // the Records list gets 40% of the width (drag the bar to change)

		connect.WhenAction = THISBACK(Connect);
		pull.WhenAction = THISBACK(Pull);
		openJt.WhenAction = THISBACK(OpenJText);
		saveJt.WhenAction = THISBACK(SaveJText);
		saveAs.WhenAction = THISBACK(SaveJTextAs);
		mergesert.WhenAction = THISBACK(Mergesert);
		templatesBtn.WhenAction = THISBACK(ShowTemplates);

		Status("Open a jText file, or connect and pull a table.");
		// [--open] FILE, --lines "24, 13, 45" (preset the Lines box), --record N (start at record N)
		const Vector<String>& args = CommandLine();
		int startRecord = 0;
		for (int i = 0; i < args.GetCount(); i++) {
			if (args[i] == "--lines" && i + 1 < args.GetCount())
				lineList <<= args[++i];
			else if (args[i] == "--record" && i + 1 < args.GetCount())
				startRecord = ScanInt(args[++i]);
		}
		for (int i = 0; i < args.GetCount(); i++) {
			if (args[i] == "--open" && i + 1 < args.GetCount())
				OpenPath(args[++i]);
			else if ((args[i] == "--lines" || args[i] == "--record" || args[i] == "--self-test-xlsx") &&
			         i + 1 < args.GetCount())
				++i;
			else if (!args[i].StartsWith("-"))
				OpenPath(args[i]);
		}
		if (startRecord > 1)
			GoToRecordRow(startRecord - 1);
		// After the window opens: the cursor goes to the Filter box (not the id, where the
		// first keystroke would replace it), and a list detached last time is detached again.
		ActiveFocus(filter);   // the control that gets the focus when the window is activated
		for (int i = 0; i < args.GetCount(); i++) {
			if (args[i] == "--self-test")
				PostCallback([this] { SelfTest(); });
			if (args[i] == "--self-test-xlsx" && i + 1 < args.GetCount()) {
				const String dir = args[i + 1];
				PostCallback([this, dir] { SelfTestXlsx(dir); });
			}
		}
		PostCallback([this] {
			if (settings.detached)
				Detach();
			filter.SetFocus();
		});
	}

	void Status(const String& text) { status.SetText(text); }

	// Show the record in row i of the Records panel (the list or the grid).
	void GoToRecordRow(int i) {
		if (i < 0 || i >= shown.GetCount()) return;
		if (grid.IsShown()) {
			grid.cr = i;
			grid.WhenRow(i);
			grid.EnsureVisible();
			grid.Refresh();
		}
		else
			list.SetCursor(i);
	}

	// --- --self-test: drive the grid with real key events, then check -------------------
	// Run on a file whose grid shows every cell (e.g. --lines 2-99). Every cell is visited
	// in a slightly random order (row by row, shuffled in windows of 8; fixed seed), reached
	// with arrow keys, and given a new value by typing + Enter. Mixed in: Ctrl+N (NULL),
	// Delete (empty string), F2 + append, Ctrl+Z right after an edit, a one-value paste,
	// a refused block paste, and typing into the locked id. Then every cell is checked in the
	// session, the file is saved, reloaded with the core library and checked again.
	// Prints a summary on stdout; exit code 0 when everything matched.
	int selfTestRc = -1;

	// A key, to whichever part has it: the cell editor while editing, else the grid.
	void Press(dword key) {
		if (grid.editing)
			grid.ed.Key(key, 1);
		else
			grid.Key(key, 1);
	}
	void TypeText(const String& t) { for (int ch : t.ToWString()) Press(ch); }

	void GoToCell(int r, int c) {
		while (grid.cr < r) Press(K_DOWN);
		while (grid.cr > r) Press(K_UP);
		while (grid.cc < c) Press(K_RIGHT);
		while (grid.cc > c) Press(K_LEFT);
	}

	void SelfTest() {
		const int t0 = msecs();
		const int rows = shown.GetCount(), cols = gridCols.GetCount();
		if (!grid.IsShown() || rows == 0 || cols < 2) {
			printf("self-test: needs a file and a Lines list (e.g. --lines 2-99)\n");
			selfTestRc = 2;
			Break();
			return;
		}
		// expected[r][c]: the text a cell should hold; "\x01" means NULL
		Vector<Vector<String>> expected;
		expected.SetCount(rows);
		for (int r = 0; r < rows; r++)
			for (int c = 0; c < cols; c++)
				expected[r].Add(CellGrid::Shown(grid.GetCell(r, c)) == kNullShown ? String("\x01")
				                : CellGrid::Shown(grid.GetCell(r, c)) == kEmptyShown ? String() : grid.GetCell(r, c).text);
		Vector<Point> order;   // x = column, y = row; the id column is tested separately
		for (int r = 0; r < rows; r++)
			for (int c = 1; c < cols; c++)
				order.Add(Point(c, r));
		uint32 seed = 313;
		auto rnd = [&seed](int n) { seed = seed * 1103515245u + 12345u; return (int)((seed >> 16) % (uint32)n); };
		for (int i = 0; i + 1 < order.GetCount(); i += 8)
			for (int k = min(i + 7, order.GetCount() - 1); k > i; k--)
				Swap(order[k], order[i + rnd(k - i + 1)]);

		int edits = 0, nulls = 0, empties = 0, appends = 0, undos = 0, pastes = 0;
		Vector<Vector<String>> ops;   // per cell: what the test did to it, for a mismatch report
		ops.SetCount(rows);
		for (auto& o : ops) o.SetCount(cols);
		grid.MoveTo(0, 0);
		// the locked id refuses typing
		const String id0 = expected[0][0];
		TypeText("999999");
		Press(K_ENTER);
		const bool idKept = grid.GetCell(0, 0).text == id0;
		for (int i = 0; i < order.GetCount(); i++) {
			// Let the window handle its events now and then: a Wayland desktop drops a program
			// that leaves its messages unread for long (and the grid can be watched working).
			if (i % 50 == 0)
				ProcessEvents();
			const int r = order[i].y, c = order[i].x;
			GoToCell(r, c);
			const int f = gridCols[c];
			String want;
			if (i % 50 == 7) {           // Ctrl+N: NULL
				Press(K_CTRL|K_N);
				want = "\x01";
				nulls++;
			}
			else if (i % 53 == 11) {     // Delete on the whole value: empty string
				Press(K_DELETE);
				Press(K_ENTER);
				want = String();
				empties++;
			}
			else if (i % 41 == 5 && expected[r][c] != "\x01" && !expected[r][c].IsEmpty()) {
				Press(K_F2);              // letter by letter: append to the end
				TypeText("+x");
				Press(K_ENTER);
				want = expected[r][c] + "+x";
				appends++;
			}
			else if (i % 211 == 17) {    // a one-value paste (with Excel's line break)
				WriteClipboardText(Format("P%d.%d\r\n", shown[r] + 1, f + 1));
				Press(K_CTRL|K_V);
				Press(K_ENTER);
				want = Format("P%d.%d", shown[r] + 1, f + 1);
				pastes++;
			}
			else {
				TypeText(Format("R%dL%d", shown[r] + 1, f + 1));
				Press(K_ENTER);
				want = Format("R%dL%d", shown[r] + 1, f + 1);
			}
			expected[r][c] = want;
			ops[r][c] << (want == "\x01" ? "N" : want.IsEmpty() ? "D" : appends && want.EndsWith("+x") ? "A" : "T")
			           << Format("@%d ", i);
			edits++;
			if (i % 97 == 3) {           // Ctrl+Z right away: the cell gets its old value back
				// (the cursor is on the next cell, with nothing typed, so this undoes the save)
				const String prev = undo.GetCount() ? (undo.Top().was_null ? String("\x01") : String(undo.Top().text.c_str())) : want;
				Press(K_CTRL|K_Z);
				expected[r][c] = prev;
				ops[r][c] << "Z ";
				undos++;
			}
		}
		// a block paste is refused: the cell keeps its value
		GoToCell(0, 1);
		const String keep = expected[0][1];
		WriteClipboardText("a\tb\r\nc\td\r\n");
		Press(K_CTRL|K_V);
		Press(K_ENTER);
		const bool blockRefused = (keep == "\x01" ? grid.GetCell(0, 1).kind == CellGrid::NUL : grid.GetCell(0, 1).text == keep);

		auto show = [](const String& v) { return v == "\x01" ? String("{Null}") : "\"" + v + "\""; };
		auto check = [&](const std::vector<jtext_gui::Record>& rowsNow, const char* what) {
			int bad = 0;
			for (int r = 0; r < rows; r++)
				for (int c = 0; c < cols; c++) {
					const auto& rec = rowsNow[(std::size_t)shown[r]];
					const auto f = (std::size_t)gridCols[c];
					const String got = jtext_gui::field_is_null(rec, f) ? String("\x01") : S(jtext_gui::field_text(rec, f));
					if (got != expected[r][c] && bad++ < 5)
						printf("  %s mismatch: record %d line %d: want %s got %s (ops: %s)\n", what, shown[r] + 1,
						       (int)f + 1, ~show(expected[r][c]), ~show(got), ~ops[r][c]);
				}
			return bad;
		};
		const int badSession = check(session.rows, "session");
		const String path = S(session.jtext_path);
		WriteJText(path, false);
		auto back = jtext_gui::jtext_load(Std(path));
		const int badFile = back ? check(back->rows, "file") : -1;
		printf("self-test: %d records x %d lines (%d cells edited by keys) in %.1f s\n", rows, cols,
		       edits, msecs(t0) / 1000.0);
		printf("  typed %d, Ctrl+N %d, Delete %d, F2+append %d, Ctrl+Z %d, paste %d\n",
		       edits - nulls - empties - appends - pastes, nulls, empties, appends, undos, pastes);
		printf("  locked id kept: %s; block paste refused: %s\n", idKept ? "yes" : "NO", blockRefused ? "yes" : "NO");
		printf("  session mismatches: %d; saved file mismatches: %d\n", badSession, badFile);
		fflush(stdout);
		selfTestRc = (badSession == 0 && badFile == 0 && idKept && blockRefused) ? 0 : 1;
		Break();
	}

	// --- the Records grid -------------------------------------------------------------
	void SetupGrid() {
		grid.GetRows = [this] { return shown.GetCount(); };
		grid.GetCols = [this] { return gridCols.GetCount(); };
		grid.GetTitle = [this](int c) {
			const int f = gridCols[c];
			const String name = FieldName(f);
			return f == 0 ? String("id") : name.IsEmpty() ? AsString(f + 1) : Format("%d %s", f + 1, name);
		};
		grid.GetWidthChars = [this](int c) { return c < gridWidths.GetCount() ? gridWidths[c] : 8; };
		grid.GetRowTitle = [this](int r) { return AsString(shown[r] + 1); };
		grid.GetCell = [this](int r, int c) {
			CellGrid::Cell cell;
			const auto& rec = session.rows[(std::size_t)shown[r]];
			const auto f = (std::size_t)gridCols[c];
			if (jtext_gui::field_is_null(rec, f))
				cell.kind = CellGrid::NUL;
			else if (jtext_gui::field_text(rec, f).empty())
				cell.kind = CellGrid::EMPTY;
			else
				cell.text = S(rec.fields[f]);
			return cell;
		};
		grid.IsLocked = [this](int, int c) { return gridCols[c] == 0 && idLocked; };
		grid.Commit = [this](int r, int c, const String& text, bool null) { return GridCommit(r, c, text, null); };
		grid.UndoLast = [this] { return GridUndo(); };
		grid.WhenRow = [this](int r) {
			if (r >= 0 && r < shown.GetCount() && shown[r] != loaded) {
				SaveCurrent();
				LoadRow(shown[r]);
			}
		};
		grid.WhenStatus = [this](const String& t) { Status(t); };
	}

	// A grid cell was changed: through the session, so Save and Mergesert see it.
	bool GridCommit(int r, int c, const String& text, bool null) {
		const int row = shown[r];
		const int f = gridCols[c];
		const auto& rec = session.rows[(std::size_t)row];
		std::string value;
		if (!null) {
			if (text == kNullShown)
				null = true;
			else
				value = text == kEmptyShown ? std::string{} : Std(text);
		}
		if (f == 0 && (null || !jtext_gui::parse_record_id(value))) {
			Status("Line 1 is the record id. It has to be an integer.");
			return false;
		}
		UndoStep& u = undo.Add();
		u.row = row;
		u.field = f;
		u.was_null = jtext_gui::field_is_null(rec, (std::size_t)f);
		u.had_line = jtext_gui::field_has_line(rec, (std::size_t)f);
		u.text = jtext_gui::field_text(rec, (std::size_t)f);
		if (null)
			session.set_null((std::size_t)row, (std::size_t)f);
		else
			session.set_field((std::size_t)row, (std::size_t)f, value);
		AfterGridChange(row, f);
		return true;
	}

	// Ctrl+Z in the grid: put back the newest saved cell change, and go to that cell.
	bool GridUndo() {
		if (undo.IsEmpty()) return false;
		const UndoStep u = undo.Top();
		undo.Drop();
		if (u.row >= (int)session.rows.size()) return false;
		if (u.was_null)   // NULL, whether or not the file had a NULL line for it
			session.set_null((std::size_t)u.row, (std::size_t)u.field);
		else
			session.set_field((std::size_t)u.row, (std::size_t)u.field, u.text);
		const int gr = FindIndex(shown, u.row), gc = FindIndex(gridCols, u.field);
		if (gr >= 0 && gc >= 0) {
			grid.cr = gr;
			grid.cc = gc;
		}
		AfterGridChange(u.row, u.field);
		Status(Format("Undone: record %d, line %d.", u.row + 1, u.field + 1));
		return true;
	}

	// After a grid edit: on the right, update only the line that changed (reloading the
	// whole record redrew every line field, up to 99, for each edit).
	void AfterGridChange(int row, int f) {
		if (row == loaded) {
			const auto& rec = session.rows[(std::size_t)row];
			loading = true;
			for (int i = 0; i < visible && i < lineOf.GetCount(); i++)
				if (lineOf[i] == f) {
					field[i] <<= ShowField(rec, f);
					String mk = ShowMark(rec, f);
					fieldMark[i] <<= TrimBoth(mk).IsEmpty() ? DefaultMarker() : mk;
					fieldComment[i] <<= ShowComment(rec, f);
				}
			loading = false;
			ShowFieldInfo();
		}
		NoteEdit();
		grid.Refresh();
	}

	// Ctrl+N in a line field on the right: that line becomes NULL.
	void SetRowNull(int i) {
		if (i >= visible || i >= lineOf.GetCount()) return;
		if (lineOf[i] == 0) {
			Status("Line 1 is the record id; it cannot be NULL.");
			return;
		}
		field[i] <<= kNullShown;
		field[i].SetSelection();
		OnEdit();
	}

	void ToggleIdLock() {
		SaveCurrent();
		idLocked = !idLocked;
		unlockId.SetLabel(idLocked ? "Unlock Row ID" : "Lock Row ID");
		if (field.GetCount())
			field[0].SetEditable(!idLocked);
		if (grid.editing)
			grid.EndEdit(false);
		grid.Refresh();
		Status(idLocked ? "Record ids are locked." : "Record ids can be changed. Lock Row ID when done.");
	}

	void Detach() {
		if (detached) return;
		splitPos = split.GetPos();
		split.Remove(listPane);            // the right side takes the whole width
		recordsWin.Add(listPane.SizePos());
		const Rect saved(settings.list_x, settings.list_y, settings.list_x + settings.list_cx,
		                 settings.list_y + settings.list_cy);
		if (GuiSettings::Usable(saved, Size(Zx(200), Zy(150))))
			recordsWin.SetRect(saved);
		else {
			const Rect work = GetWorkArea();
			recordsWin.SetRect(work.CenterRect(work.Width() / 2, work.Height() * 8 / 10));
		}
		// OpenMain: a normal window with no owner. Plain Open() would make the main window
		// its owner, and GTK would then treat it as a dialog, which GNOME does not
		// maximize (title-bar double-click) and keeps tied to the main window.
		recordsWin.OpenMain();
		detachBtn.SetLabel("Attach");
		detachBtn.Tip("Put the Records list back into this window (closing its window does the same)");
		detached = true;
		Status("Records list in its own window. Attach, or close that window, to put it back.");
	}

	void Attach() {
		if (!detached) return;
		KeepListWindowPlace();
		listPane.Remove();
		recordsWin.CloseNow();
		split.Set(listPane, fields);
		split.SetPos(splitPos);
		detachBtn.SetLabel("Detach");
		detachBtn.Tip("Move the Records list into a window of its own (e.g. on another monitor)");
		detached = false;
		Status("Records list back in the main window.");
	}

	void KeepListWindowPlace() {
		if (!recordsWin.IsOpen() || recordsWin.IsMaximized()) return;
		const Rect r = recordsWin.GetRect();
		settings.list_x = r.left;
		settings.list_y = r.top;
		settings.list_cx = r.Width();
		settings.list_cy = r.Height();
	}

	void StoreSettings() {
		settings.maximized = IsMaximized();
		if (!settings.maximized) {   // keep the normal size of a maximized window
			const Rect r = GetRect();
			settings.x = r.left;
			settings.y = r.top;
			settings.cx = r.Width();
			settings.cy = r.Height();
		}
		settings.detached = detached;
		settings.split = detached ? splitPos : split.GetPos();
		if (detached)
			KeepListWindowPlace();
		StoreAsJsonFile(settings);
	}

	static constexpr int kRowStep = 30;   // pixels (unzoomed) from one line row to the next
	static constexpr int kCommentWidth = 220;   // pixels (unzoomed): a line's comment box

	// Create rows up to n (they stay hidden until SetVisibleRows shows them).
	void EnsureRows(int n) {
		Font mono = Monospace(StdFont().GetHeight());
		while (field.GetCount() < n) {
			const int i = field.GetCount();
			const int yy = 6 + i * kRowStep;
			Label& lb = num.Add();
			EditString& mk = fieldMark.Add();
			FieldEdit& fd = field.Add();
			EditString& cm = fieldComment.Add();
			fd.WhenNull = [this, i] { SetRowNull(i); };
			lb.SetText(i == 0 ? String("id") : Format("%3d.", i + 1));
			lb.SetFont(mono);
			mk.SetFont(mono);
			mk.WhenAction = THISBACK(OnEdit);
			fd.WhenAction = THISBACK(OnEdit);
			cm.WhenAction = THISBACK(OnEdit);
			cm.NullText("comment");
			mk.NoWantFocus();
			fd.WantFocus();
			lines.content.Add(lb.LeftPosZ(8, 36).TopPosZ(yy, 22));
			lines.content.Add(mk.LeftPosZ(48, 84).TopPosZ(yy - 2, 24));
			lines.content.Add(fd.HSizePosZ(140, kCommentWidth + 16).TopPosZ(yy - 2, 24));
			lines.content.Add(cm.RightPosZ(8, kCommentWidth).TopPosZ(yy - 2, 24));
		}
	}

	// Show rows 0..n-1 (1 to kMaxFields); hide the rest. Row i shows field lineOf[i].
	void SetVisibleRows(int n) {
		n = clamp(n, 1, (int)jtext_gui::kMaxFields);
		EnsureRows(n);
		for (int i = 0; i < field.GetCount(); i++) {
			const bool show = i < n;
			num[i].Show(show);
			fieldMark[i].Show(show);
			field[i].Show(show);
			fieldComment[i].Show(show);
		}
		visible = n;
		lines.SetContentHeight(Zy(6 + n * kRowStep), Zy(kRowStep));
	}

	// The + Line button: one more line on the current record, shown as {NULL} (absent
	// until something is typed) and selected, so typing replaces it.
	void AddLine() {
		if (loaded < 0 || loaded >= (int)session.rows.size()) return;
		if (!TrimBoth(Field(lineList)).IsEmpty()) {
			Status("+ Line adds to the whole record: clear the Lines box first.");
			return;
		}
		if (visible >= (int)jtext_gui::kMaxFields) {
			Status(Format("A record has at most %d lines.", (int)jtext_gui::kMaxFields));
			return;
		}
		const int i = visible;
		lineOf.Add(i);
		SetVisibleRows(visible + 1);
		SetRowLabel(i);
		loading = true;
		field[i] <<= kNullShown;
		fieldMark[i] <<= DefaultMarker();
		fieldComment[i] <<= String();
		loading = false;
		field[i].SetFocus();
		field[i].SetSelection();
		Status(Format("Line %d added. Type its value; left as {NULL} it is not saved.", i + 1));
	}

	// Row i's number label: "id" for line 1, else the line number.
	void SetRowLabel(int i) {
		const int f = lineOf[i];
		if (f == 0)
			num[i].SetText("id");
		else if (f < (int)session.column_names.size())
			num[i].SetText(Format("%3d", f + 1));
		else
			num[i].SetText(Format("%3d.", f + 1));
	}

	// Field f's name and definition: a database column, else the jText Fields block.
	const jtext_gui::ColumnDef* FieldDef(int f) const {
		if (f >= 0 && f < (int)session.columns.size())
			return &session.columns[(std::size_t)f];
		if (f >= 0 && f < (int)session.field_defs.size())
			return &session.field_defs[(std::size_t)f];
		return nullptr;
	}

	String FieldName(int f) const {
		const jtext_gui::ColumnDef* def = FieldDef(f);
		if (def && !def->name.empty())
			return S(def->name);
		if (f >= 0 && f < (int)session.column_names.size())
			return S(session.column_names[(std::size_t)f]);
		return String();
	}

	// The field named name (any case), or -1.
	int FieldByName(const String& name) const {
		const String want = ToLower(name);
		const int n = (int)std::max({session.columns.size(), session.field_defs.size(),
		                             session.column_names.size()});
		for (int f = 0; f < n; f++)
			if (ToLower(FieldName(f)) == want)
				return f;
		return -1;
	}

	// The Lines box as field indexes (after the id), separated by commas: line numbers
	// ("24"), ranges ("5-9") and field names ("price"), in the order written. Duplicates
	// and line 1 (always shown first) are dropped. Empty = all lines. False with a
	// message for anything that is none of those.
	bool ParseLineList(const String& text, Vector<int>& out, String& error) {
		out.Clear();
		Index<int> seen;
		seen.Add(0);
		auto take = [&](int f) {
			if (seen.Find(f) < 0) {
				seen.Add(f);
				out.Add(f);
			}
		};
		String t = text;
		t.Replace(";", ",");
		for (String part : Split(t, ',')) {
			part = TrimBoth(part);
			if (part.IsEmpty()) continue;
			const int named = FieldByName(part);   // a whole name first: names may hold '-'
			if (named >= 0) {
				take(named);
				continue;
			}
			int dash = part.Find('-');
			int a = 0, b = 0;
			bool ok;
			if (dash > 0) {
				String sa = TrimBoth(part.Left(dash)), sb = TrimBoth(part.Mid(dash + 1));
				ok = !sa.IsEmpty() && !sb.IsEmpty() && IsDigit(sa[0]) && IsDigit(sb[0]);
				a = ok ? ScanInt(sa) : 0;
				b = ok ? ScanInt(sb) : 0;
			}
			else {
				ok = IsDigit(part[0]);
				a = b = ok ? ScanInt(part) : 0;
			}
			const int top = (int)jtext_gui::kMaxFields;
			if (!ok || IsNull(a) || IsNull(b) || a < 1 || b < 1 || a > top || b > top) {
				error = Format("Lines: \"%s\" is not a line number (1-%d), a range like 5-9, "
				               "or a field name.", part, top);
				return false;
			}
			const int step = a <= b ? 1 : -1;
			for (int n = a; ; n += step) {
				take(n - 1);
				if (n == b) break;
			}
		}
		return true;
	}

	// Which lines the rows show for this record: the id, then the Lines list; or, with no
	// list, every line the record or the table has (at least kDefaultFields).
	void ComputeLines(const jtext_gui::Record& rec) {
		Vector<int> picked;
		String error;
		const String text = TrimBoth(Field(lineList));
		lineOf.Clear();
		if (!text.IsEmpty() && ParseLineList(text, picked, error)) {
			lineOf.Add(0);
			lineOf.Append(picked);
			return;
		}
		if (!error.IsEmpty())
			Status(error);
		const int n = std::max({(int)jtext_gui::kDefaultFields, (int)rec.size(),
		                        (int)session.column_names.size()});
		for (int i = 0; i < n; i++)
			lineOf.Add(i);
	}

	void OnLineList() {
		if (loading) return;
		SaveCurrent();
		Vector<int> picked;
		String error;
		const String text = TrimBoth(Field(lineList));
		if (!text.IsEmpty() && !ParseLineList(text, picked, error)) {
			Status(error);   // keep showing the last good choice while the text is being typed
			return;
		}
		if (loaded >= 0)
			LoadRow(loaded);
		RefreshList();
		Status(text.IsEmpty() ? String("Showing all lines.")
		                      : Format("Showing the id and %d chosen line(s) on every record.", picked.GetCount()));
	}

	void OnRowFocus(Ctrl& c) {
		for (int i = 0; i < visible; i++)
			if (&c == &field[i] || &c == &fieldMark[i] || &c == &fieldComment[i]) {
				focusRow = i;
				ShowFieldInfo();
				return;
			}
	}

	// The bottom box: "24 - name | String(80), Not Null | value: 7 characters".
	void ShowFieldInfo() {
		if (focusRow < 0 || focusRow >= visible || focusRow >= lineOf.GetCount()) {
			fieldInfo <<= String();
			return;
		}
		const int f = lineOf[focusRow];
		const jtext_gui::ColumnDef* def = FieldDef(f);
		const String name = FieldName(f);
		String text = Format("%d - %s", f + 1, name.IsEmpty() ? String("(no field name)") : name);
		if (def) {
			String type = S(def->jtext_type.empty() ? def->sql_type : def->jtext_type);
			if (def->length > 0)
				type << "(" << def->length << ")";
			if (!def->sql_type.empty() && !def->jtext_type.empty())
				type << " [" << S(def->sql_type) << "]";
			text << "  |  " << (type.IsEmpty() ? String("no type") : type)
			     << ", " << (def->not_null ? "Not Null" : "Nullable");
			if (def->primary_key)
				text << ", primary key";
		}
		const String raw = Field(field[focusRow]);
		text << "  |  value: ";
		if (raw == kNullShown)
			text << "NULL";
		else if (raw == kEmptyShown || raw.IsEmpty())
			text << "empty string";
		else {
			const int chars = ToUtf32(raw).GetCount();
			text << chars << (chars == 1 ? " character" : " characters");
			if (def && def->length > 0 && chars > def->length)
				text << Format("  (longer than %d!)", def->length);
		}
		if (def && def->not_null && raw == kNullShown)
			text << "  (Not Null field is NULL!)";
		fieldInfo <<= text;
	}

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

	// The Section list follows the session: a jText file's sections, the open one picked.
	void FillSections() {
		fillingSections = true;
		section.Clear();
		for (const auto& name : session.jtext_sections)
			section.Add(S(name));
		if (!session.table.empty())
			section <<= S(session.table);
		const bool show = session.kind == jtext_gui::SessionKind::JTextFile && session.jtext_sections.size() > 1;
		lSection.Show(show);
		section.Show(show);
		fillingSections = false;
	}

	// Another section was picked: offer to save this one's edits, then open that one.
	// Saving writes only this section's data; the rest of the file stays as it is.
	void OnSection() {
		if (fillingSections) return;
		const String want = ~section;
		if (want.IsEmpty() || Std(want) == session.table) return;
		SaveCurrent();
		if (session.is_dirty()) {
			const int answer = PromptSaveDontSaveCancel(
				"Save changes to section '" + S(session.table) + "' before opening '" + want + "'?");
			if (answer == 1)
				SaveJText();
			if (answer != 0 && (answer != 1 || session.is_dirty())) {
				fillingSections = true;
				section <<= S(session.table);   // stay on this section
				fillingSections = false;
				return;
			}
		}
		OpenPath(S(session.jtext_path), want);
	}

	void FinishLoad(const String& note) {
		FillSections();
		undo.Clear();   // grid undo belongs to the records that were loaded before
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
		StoreSettings();
		if (detached)
			recordsWin.CloseNow();
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
		for (int i = 0; i < visible && i < lineOf.GetCount(); i++) {
			const auto fi = (std::size_t)lineOf[i];   // the field this row edits
			session.set_line_comment(row, fi, Std(TrimBoth(Field(fieldComment[i]))));
			if (fi == 0) {
				ApplyRecordId(row, def);
				continue;
			}
			String raw = Field(field[i]);
			if (raw == kNullShown) {
				if (!jtext_gui::field_is_null(session.rows[row], fi))
					session.set_null(row, fi);
				continue;
			}
			String mk = TrimBoth(Field(fieldMark[i]));
			if (mk.IsEmpty()) {
				const auto& rec = session.rows[row];
				if (jtext_gui::field_has_line(rec, fi))
					mk = S(jtext_gui::field_marker(rec, fi));
				if (mk.IsEmpty())
					mk = def;
				loading = true;
				fieldMark[i] <<= mk;
				loading = false;
			}
			std::string value = raw == kEmptyShown ? std::string{} : Std(raw);
			session.set_field(row, fi, value);
			session.set_field_marker(row, fi, Std(mk));
		}
	}

	// The comment on the line that starts at field f ("" when it has none).
	static String ShowComment(const jtext_gui::Record& rec, int f) {
		const auto it = rec.line_comments.find((std::size_t)f);
		return it == rec.line_comments.end() ? String() : S(it->second);
	}

	void LoadRow(int index) {
		if (index < 0 || index >= (int)session.rows.size()) return;
		loading = true;
		const auto& rec = session.rows[(std::size_t)index];
		if (TrimBoth(Field(marker)).IsEmpty())
			marker <<= jtext_gui::kDefaultMarker;
		String def = DefaultMarker();
		// The id, then the Lines list; or every line the record or table has (at least 10).
		ComputeLines(rec);
		SetVisibleRows(lineOf.GetCount());
		for (int i = 0; i < visible; i++) {
			const int f = lineOf[i];
			field[i] <<= ShowField(rec, f);
			String mk = ShowMark(rec, f);
			if (TrimBoth(mk).IsEmpty())
				mk = def;
			fieldMark[i] <<= mk;
			fieldComment[i] <<= ShowComment(rec, f);
			SetRowLabel(i);
		}
		loaded = index;
		if (field.GetCount())
			field[0].SetEditable(!idLocked);
		loading = false;
		ShowFieldInfo();
	}

	// s padded with spaces (or cut, ending in '~') to exactly w characters.
	static String PadTo(const String& s, int w) {
		WString t = s.ToWString();
		if (t.GetCount() > w)
			t = t.Left(max(w - 1, 0)) + WString("~");
		return t.ToString() + String(' ', max(w - t.GetCount(), 0));
	}

	// Make the Records column cx wide (at least the panel's width). The header ignores a
	// width while it has no size yet, so before the window is laid out it is set just after.
	void ListWidth(int cx) {
		HeaderCtrl& h = list.HeaderObject();
		if (h.GetSize().cx > 0)
			h.SetTabWidth(0, max(cx, list.GetSize().cx - Zx(20)));
		else
			PostCallback([=, this] { ListWidth(cx); });
	}

	// The Records list, in the monospace font so values line up. With a Lines list, each
	// record shows its id and chosen values, every value padded to its column's width.
	void RefreshList() {
		int keep = loaded;
		int c = list.GetCursor();
		if (c >= 0 && c < shown.GetCount())
			keep = shown[c];
		String filt = Field(filter);
		Vector<int> chosen;   // the Lines list, when there is a good one
		String error;
		if (!TrimBoth(Field(lineList)).IsEmpty() && !ParseLineList(Field(lineList), chosen, error))
			chosen.Clear();
		loading = true;
		list.Clear();
		shown.Clear();
		int cursor = -1;
		for (int i = 0; i < (int)session.rows.size(); i++) {
			if (!RowMatchesFilter(session.rows[(std::size_t)i], filt, session.case_sensitive))
				continue;
			if (i == keep)
				cursor = shown.GetCount();
			shown.Add(i);
		}
		// Column widths over the rows shown: the record number, the id, each chosen value
		// (values longer than kListValueMax are cut in the list, not in the record).
		const int kListValueMax = 30;
		const int numW = AsString(max((int)session.rows.size(), 1)).GetCount();
		int idW = 2;
		Vector<int> w;
		w.SetCount(chosen.GetCount(), 0);
		for (int i : shown) {
			const auto& r = session.rows[(std::size_t)i];
			idW = max(idW, min(S(jtext_gui::field_text(r, 0)).ToWString().GetCount(), kListValueMax));
			for (int k = 0; k < chosen.GetCount(); k++)
				w[k] = max(w[k], min(ShowField(r, chosen[k]).ToWString().GetCount(), kListValueMax));
		}
		if (!chosen.IsEmpty()) {   // the grid: the id, then the chosen lines
			gridCols.Clear();
			gridCols.Add(0);
			gridCols.Append(chosen);
			gridWidths.Clear();
			gridWidths.Add(idW);
			gridWidths.Append(w);
			for (int& gw : gridWidths)
				gw = max(gw, (int)String(kNullShown).GetCount());
			grid.cr = max(cursor, 0);
			list.Hide();
			grid.Show();
			grid.Reset();
			loading = false;
			return;
		}
		grid.Hide();
		list.Show();
		const Font mono = Monospace(StdFont().GetHeight());
		int widest = 0;   // characters
		for (int i : shown) {
			const auto& r = session.rows[(std::size_t)i];
			String line = PadTo(AsString(i + 1), numW) + ". ";
			if (chosen.IsEmpty())
				line << S(r.marker) << ' ' << S(jtext_gui::record_preview(r));
			else {
				line << "id " << PadTo(S(jtext_gui::field_text(r, 0)), idW);
				for (int k = 0; k < chosen.GetCount(); k++)
					line << " | " << (chosen[k] + 1) << ": " << PadTo(ShowField(r, chosen[k]), w[k]);
			}
			widest = max(widest, line.ToWString().GetCount());
			list.Add(AttrText(line).SetFont(mono));
		}
		// Wide rows get a horizontal scroll bar instead of being cut at the panel's edge.
		ListWidth(GetTextSize("0", mono).cx * widest + Zx(16));
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
		ShowFieldInfo();   // the value's length changes as it is typed
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

	void OpenPath(const String& path, const String& sectionName = String()) {
		SaveCurrent();
		auto r = jtext_gui::session_open_jtext(session, Std(path), Std(sectionName));
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
		if (session.jtext_sections.size() > 1)
			note << ", section " << S(session.table);
		for (const auto& w : session.jtext_warnings)
			note << ". Warning: " << S(w);
		if (templateNames.GetCount())
			note << ". " << templateNames.GetCount() << " templates — use Templates to read them.";
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
		// The open section's templates (each configuration can have its own).
		for (const auto& sec : parsed->sections) {
			if (!session.table.empty() && sec.name != session.table)
				continue;
			for (const auto& t : sec.templates) {
				templateNames.Add(S(t.name));
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
			                               shape.table.empty() ? "records" : shape.table, use,
			                               session.case_sensitive, session.end_comments);
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

	// --- Excel (.xlsx) ------------------------------------------------------------------
	// Fields any record or the table has: up to the last one with a name, a value or a line.
	std::size_t UsedFields() const {
		std::size_t n = std::max({session.columns.size(), session.field_defs.size(), session.column_names.size()});
		for (const auto& rec : session.rows)
			for (std::size_t f = rec.size(); f > n; f--)
				if (!rec.is_null[f - 1] || rec.has_line[f - 1]) {
					n = f;
					break;
				}
		return max(n, std::size_t{1});
	}

	void ExportXlsx() {
		SaveCurrent();
		if (!session.is_open() || session.rows.empty()) {
			Tell("Open a jText file or pull a table first.");
			return;
		}
		// what is on screen (the Filter's records, the Lines list's lines), or everything
		const bool filtered = shown.GetCount() != (int)session.rows.size();
		const bool someLines = grid.IsShown() && gridCols.GetCount();
		bool onScreen = false;
		if (filtered || someLines) {
			const int answer = Prompt("Export to Excel", CtrlImg::question(), DeQtf(Format(
				"Export what is on screen (%d records, %s), or everything (%d records, all lines)?",
				shown.GetCount(), someLines ? Format("%d lines", gridCols.GetCount()) : String("all lines"),
				(int)session.rows.size())), "On screen", "Everything", "Cancel");
			if (answer < 0) return;
			onScreen = answer == 1;
		}
		FileSel fs;
		fs.Type("Excel workbook", "*.xlsx");
		fs.DefaultExt("xlsx");
		String dir = session.jtext_path.empty() ? GetCurrentDirectory() : GetFileFolder(S(session.jtext_path));
		fs <<= AppendFileName(dir, SheetName() + ".xlsx");
		if (!fs.ExecuteSaveAs("Export Excel (.xlsx)")) return;
		String error;
		if (!DoExportXlsx(~fs, onScreen, error))
			Tell("Export failed:\n" + error);
	}

	String SheetName() {
		String name = TrimBoth(Field(table));
		if (name.IsEmpty()) name = S(session.table);
		return name.IsEmpty() ? String("records") : name;
	}

	// Write the records to path: what is on screen (the Filter's records, the Lines list's
	// lines) or everything. The id is always the first column.
	bool DoExportXlsx(const String& path, bool onScreen, String& error) {
		const bool someLines = grid.IsShown() && gridCols.GetCount();
		std::vector<std::size_t> rows, fields;
		if (onScreen)
			for (int i : shown) rows.push_back((std::size_t)i);
		else
			for (std::size_t i = 0; i < session.rows.size(); i++) rows.push_back(i);
		if (onScreen && someLines)
			for (int f : gridCols) fields.push_back((std::size_t)f);
		else
			for (std::size_t f = 0; f < UsedFields(); f++) fields.push_back(f);
		const jtext_gui::Sheet sheet = jtext_gui::session_to_sheet(session, rows, fields);
		std::vector<jtext_gui::CellType> types;
		types.push_back(jtext_gui::field_cell_type(session, 0));
		for (std::size_t f : fields)
			if (f != 0) types.push_back(jtext_gui::field_cell_type(session, f));
		auto r = jtext_gui::write_xlsx_file(Std(path), sheet, types, Std(SheetName()));
		if (!r) {
			error = S(r.error());
			return false;
		}
		Status(Format("Exported %d records x %d fields to %s.", (int)sheet.rows.size() - 1,
		              (int)sheet.rows[0].size(), path));
		return true;
	}

	// Merge a workbook's first sheet into the records by record id (a preview first).
	void ImportXlsx() {
		SaveCurrent();
		if (!session.is_open()) {
			Tell("Open a jText file or pull a table first: an import merges into its records.");
			return;
		}
		FileSel fs;
		fs.Type("Excel workbook", "*.xlsx");
		fs.AllFilesType();
		if (!session.jtext_path.empty()) fs.ActiveDir(GetFileFolder(S(session.jtext_path)));
		if (!fs.ExecuteOpen("Import Excel (.xlsx)")) return;
		const String path = ~fs;
		auto sheet = jtext_gui::read_xlsx_file(Std(path));
		if (!sheet) {
			Tell("Could not read the workbook:\n" + S(sheet.error()));
			return;
		}
		auto preview = jtext_gui::import_sheet(session, *sheet, false);
		if (!preview) {
			Tell("Cannot import " + GetFileName(path) + ":\n" + S(preview.error()));
			return;
		}
		String text = Format("Import %s into these records?\n\n"
		                     "%d records updated (%d values)\n%d records added\n%d rows skipped\n"
		                     "%d columns used (matched by name, the id first)",
		                     GetFileName(path), preview->records_updated, preview->values_changed,
		                     preview->records_added, preview->rows_skipped, preview->columns_used);
		for (std::size_t i = 0; i < preview->notes.size() && i < 8; i++)
			text << "\n- " << S(preview->notes[i]);
		text << "\n\nAn empty cell is an empty string. Ids are not changed; nothing is deleted. "
		        "The changes are unsaved edits until you save.";
		if (preview->records_updated == 0 && preview->records_added == 0) {
			PromptOK(DeQtfLf(text + "\n\nNothing to import: the records already match."));
			return;
		}
		if (!PromptOKCancel(DeQtfLf(text))) return;
		String error;
		if (!ApplyImport(*sheet, GetFileName(path), error))
			Tell("Import failed:\n" + error);
	}

	bool ApplyImport(const jtext_gui::Sheet& sheet, const String& name, String& error) {
		auto done = jtext_gui::import_sheet(session, sheet, true);
		if (!done) {
			error = S(done.error());
			return false;
		}
		undo.Clear();   // grid undo does not cover an import (reload the file to undo it)
		RefreshList();
		LoadRow(loaded >= 0 && loaded < (int)session.rows.size() ? loaded : 0);
		NoteEdit();
		Status(Format("Imported %s: %d records updated (%d values), %d added. Unsaved until you save.",
		              name, done->records_updated, done->values_changed, done->records_added));
		return true;
	}

	// --self-test-xlsx DIR: export everything and what is on screen, re-import the full
	// export unchanged (expect no change), then import an edited copy (one changed cell,
	// one emptied cell, one new record) and check the records and the right side.
	void SelfTestXlsx(const String& dir) {
		int bad = 0;
		auto check = [&](bool ok, const char *what) {
			printf("  %s %s\n", ok ? "ok  " : "FAIL", what);
			if (!ok) bad++;
		};
		String error;
		const String all = AppendFileName(dir, "all.xlsx"), screen = AppendFileName(dir, "screen.xlsx");
		check(DoExportXlsx(all, false, error), "export everything");
		check(DoExportXlsx(screen, true, error), "export what is on screen");
		auto sheetAll = jtext_gui::read_xlsx_file(Std(all));
		auto sheetScreen = jtext_gui::read_xlsx_file(Std(screen));
		check(sheetAll && sheetAll->rows.size() == session.rows.size() + 1, "all records exported");
		check(sheetScreen && sheetScreen->rows.size() == (std::size_t)shown.GetCount() + 1 &&
		      (!grid.IsShown() || sheetScreen->rows[0].size() == (std::size_t)gridCols.GetCount()),
		      "on screen: the shown records and the Lines list's columns");
		if (sheetAll) {
			auto again = jtext_gui::import_sheet(session, *sheetAll, false);
			check(again && again->records_updated == 0 && again->records_added == 0,
			      "re-importing the unchanged export changes nothing");
			// an edited copy: row 2 column 2 changed, row 3 column 2 emptied, a new record
			jtext_gui::Sheet edited = *sheetAll;
			if (edited.rows.size() > 3 && edited.rows[0].size() > 2) {
				edited.rows[2][1] = "Edited in Excel";
				edited.rows[3][1].clear();
				std::vector<std::string> extra(edited.rows[0].size());
				extra[0] = "9999";
				extra[1] = "Added in Excel";
				edited.rows.push_back(extra);
				const int before = (int)session.rows.size();
				check(ApplyImport(edited, "edited copy", error), "import an edited copy");
				const auto& r1 = session.rows[1];
				const auto& r2 = session.rows[2];
				check(jtext_gui::field_text(r1, 1) == "Edited in Excel", "the changed cell is in record 2");
				check(!jtext_gui::field_is_null(r2, 1) && jtext_gui::field_text(r2, 1).empty(),
				      "the emptied cell is an empty string, not NULL");
				check((int)session.rows.size() == before + 1 &&
				      jtext_gui::field_text(session.rows.back(), 0) == "9999", "record 9999 added");
				check(session.is_dirty(), "the import is an unsaved edit");
				LoadRow(1);
				bool shownRight = false;
				for (int i = 0; i < visible; i++)
					if (lineOf[i] == 1 && Field(field[i]) == "Edited in Excel") shownRight = true;
				check(shownRight, "the right side shows the imported value");
			}
		}
		printf("self-test-xlsx: %s\n", bad ? "FAILED" : "all good");
		fflush(stdout);
		selfTestRc = bad ? 1 : 0;
		Break();
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
	JTextGui w;
	w.Run();
	if (w.selfTestRc >= 0)
		SetExitCode(w.selfTestRc);
}
