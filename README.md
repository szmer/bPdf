bPdf is a PDF framework. That is, it automates low-level tasks as extracting objects or looking up indirect references. There are some libraries which are almost readers/writers without GUI. In contrary, bPdf is designed to simplify tricky extractions and modifications, or maybe to be a base for higher-level something.

Principles
----------
1. Providing narrow range of functions giving easy access to whole syntactic structure.
2. Keeping it compilable with [Swig](http://www.swig.org/) as scripting language module (in fact, I intend to use it as Python module). 
3. Keeping decent performance (at least when project will get more mature).
4. Be liberally-licensed (X11).

State
-----
The is very much a work in progress and you should expect nothing special. Interface changes are currently made without notice, hah.

This repository contains Pdf parser. I also wrote simple integrated writer which will be included later (now I focus on parsing).

Short todo for now:

- implement object referencing for files with xref streams, objects streams, linearized or so
- type-checking functions
- redesign some most terrible shady places in code (= object extracting)

Requirement
-----------
- [zlib](http://www.zlib.net/)
