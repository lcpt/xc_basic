This disk contains the example programs from Mark Nelson's book
"C++ Programmer's Guide to the Standard Template Library."  The examples
are contained in subdirectories corresponding to the chapter of the book
in which they are found.

The STL directory on this disk contains the July 1995 distribution of the
HP version of the STL, along with a PostScript version of the STL
specification.

As of August, 1995, no PC compilers are advertising support or shipping a
version of the STL. Borland C++ 4.5 does a fairly good job of compiling
STL programs, given a couple of workarounds.  The source in this book
will work with Borland's compiler.

Microsoft will not support the STL with Visual C++ 1.x, as they do not
support templates in their 16 bit compiler.  Microsoft will reportedly
add STL support to Visual C++ 4.0, shipping at an undetermined time in
the future.

Watcom 10.5 seems to work well with the HP code, although it hasn't been
out long enough to test thoroughly.  In order to use the examples in the
book with Watcom, you will need to make a few changes to the source.
Borland defines their string class in cstring.h, Watcom uses string.hpp.
If an example includes cstring.h, just change the name of the include file
to Watcom's string.hpp.  Additionally, Watcom uses the name "String" for
their string class instead of "string".  This means you will need to search
and replace all the occurences of "string" with "String".

Two vendors are providing commercial versions of the STL that have been
modified to work properly with many compilers, including those mentioned
above.  They are:

        ObjectSpace, Inc.
        14881 Quorum Drive
        Suite 400
        Dallas, TX  75240
        1-800-OBJECT1
        sales@objectspace.com

        ObjectSpace's product is called STL<ToolKit>

        Modena Software, Inc.
        236 N. Santa Cruz Ave
        Suite 213
        Los Gatos, CA  95030
        408-354-5616
        FAX 408-354-0846
        modena@netcom.com

        Modena's product is called STL++


