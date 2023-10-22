# sqlite_rtree_bulk_load

## Description

Fork of https://github.com/tidwall/rtree.c that implements the R\*Tree
algorithm of SQLite (which is the one of http://www.sai.msu.su/~megera/postgres/gist/papers/Rstar3.pdf,
without the forced reinsertion step), and offer fast initial creation of the
SQLite R*Tree.

## Notes

sqlite_rtree_bulk_load.c can be built either as a C99 or a C++ source code.
Performance will be slightly better when built as C++.

This is meant to be vendorized into your own code.

## Credits

- Josh Baker (@tidwall) for https://github.com/tidwall/rtree.c
- SQLite authors for the R*Tree enhancements
  https://github.com/sqlite/sqlite/blob/master/ext/rtree/rtree.c that have
  been borrowed into this repository.

## License

sqlite_rtree_bulk_load source code is available under the MIT License.
