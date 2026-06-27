# bench — compiler comparison

_Generated from `results.db`. Per compiler: median ops/sec and the low–high band, head to head; latest run per compiler._

| config | [clang22](Run_006.md)<br>median ops/sec | clang22<br>band | clang22<br>size | [gcc15](Run_025.md)<br>median ops/sec | gcc15<br>band | gcc15<br>size | [gcc16](Run_005.md)<br>median ops/sec | gcc16<br>band | gcc16<br>size |
|---|--:|:--:|--:|--:|:--:|--:|--:|:--:|--:|
| 0 flags, non-durable @10M | 7,033,660 | 6.96M–7.13M | 0 B | 11,628,331 | 11.03M–11.86M | 0 B | 7,138,792 | 6.86M–7.29M | 0 B |
| 2 flags, non-durable @10M | 7,134,671 | 6.88M–7.26M | 0 B | 11,708,883 | 11.30M–11.91M | 0 B | 7,201,516 | 7.08M–7.25M | 0 B |
| 4 flags, non-durable @10M | 7,106,454 | 6.96M–7.17M | 0 B | 11,555,651 | 10.94M–11.75M | 0 B | 7,254,559 | 7.01M–7.34M | 0 B |
| 6 flags, non-durable @10M | 7,116,311 | 6.99M–7.25M | 0 B | 11,567,481 | 11.18M–11.87M | 0 B | 7,224,176 | 7.15M–7.33M | 0 B |
| durable binary @1M | 588,036 | 490.05K–651.77K | 83.780 MB | 860,273 | 848.98K–884.47K | 83.780 MB | 620,283 | 481.40K–621.00K | 83.780 MB |
| durable binary @10M | 705,718 | 635.19K–731.20K | 837.803 MB | 916,563 | 915.05K–926.97K | 837.803 MB | 687,806 | 668.96K–731.62K | 837.803 MB |
| durable jtext @1M | 1,917,505 | 1.86M–2.01M | 82.955 MB | 1,951,162 | 1.85M–2.15M | 82.955 MB | 1,999,568 | 1.82M–2.04M | 82.955 MB |
| durable jtext @10M | 1,999,038 | 2.00M–2.10M | 848.613 MB | 2,394,138 | 2.39M–2.41M | 848.613 MB | 2,089,665 | 2.03M–2.14M | 848.613 MB |
| durable sql @1M | 423,097 | 386.04K–439.49K | 76.109 MB | 553,369 | 540.43K–563.55K | 76.117 MB | 429,445 | 364.05K–441.83K | 76.102 MB |
| durable sql @10M | 688,596 | 675.38K–708.79K | 783.043 MB | 744,592 | 734.99K–746.07K | 783.090 MB | 705,190 | 691.28K–713.28K | 782.926 MB |
