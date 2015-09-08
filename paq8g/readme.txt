PAQ8G (02.03.2006) - File archiver and compressor (based on PAQ8F).
------------------   (C) 2006, M. Mahoney (mmahoney@cs.fit.edu)
                               P. Skibinski (inikep@o2.pl)
                     http://www.ii.uni.wroc.pl/~inikep/research/paq8/paq8g.zip


1. Introduction
PAQ8G is PAQ8F with added TextFilter - a preprocessor (WRT by P. Skibinski),
which improves the compression performance, particularly on texts.

WRT (http://www.ii.uni.wroc.pl/~inikep/research/WRT45.zip) is a high-performance
file to file preprocessor NOT ONLY for textual files. It is based on StarNT,
improved version of LIPT. The main idea behind these algorithms is to replace
words with codewords (numbers) in the dictionary.

To replace we use ASCII characters from 128 up to 255 as they are rarely
used in text files. We use 4 subdictionaries, which outputs from 1 to 4
characters. Most frequent words are in the first subdictionary. Often used
words are in the second subdictionary and rarely used words are in the
third and the fourth subdictionary. 



2. Usage
To compress:
  paq8g -level archive files... (archive will be created)
or (Windows):
  dir/b | paq8g archive  (file names read from input)

level: -0 = store, -1 -2 -3 = faster (uses 21, 28, 42 MB)
-4 -5 -6 -7 -8 = smaller (uses 120, 225, 450, 900, 1800 MB)

To extract or compare:
  paq8g archive  (extracts to stored names)

To view contents: more < archive



3. History
PAQ8G (02.03.2006)
-based on PAQ8F (but Filters based on PAQ8A and no user interface to support drag and drop in Windows)
-TextFilter redesigned not to decrease compression performance on non-textual files
-PS, HLP, H, C, CPP, INI, and INF files won't be preprocessed by TextFilter
-sources can be compiled on Linux/Unix, but when using PAQ8G, dictionaries
 ("TextFilter" directory) must be present in the current working directory
-new dictionaries: WRT-short-CompSc.dic, WRT-short-Math.dic, WRT-short-Music.dic
-additional dictionaries available at http://www.ii.uni.wroc.pl/~inikep/research/dicts/:
  a) new dictionaries: WRT-ES.dic (Spanish), WRT-FI.dic (Finnish), WRT-IT.dic (Italian) 
  b) improved dictionaries: WRT-DE.dic (German), WRT-FR.dic (French), WRT-PL.dic (Polish)

PAQ8F (28.02.2006)
-a more memory efficient context model
-a new indirect context model to improve compression
-a new user interface to support drag and drop in Windows
-it does not use an English dictionary like PAQ8B/C/D/E

PAQ8E (23.02.2006)
-improved detection of XML and HTML files
-experimental support for ISO-8859-1 encoding in UTF-8
-added WRT-short-Larg.dic and removed WRT-short-MaxC.dic dictionary
-default memory changed from -0 to -4
-fixed bug from PAQ8D in Windows 95/98/Me

PAQ8D (15.02.2006)
-improved speed of file type detection (especially when creating multiple file archive)
-ExeFilter looks for a "MZ" header in a file instead of an extension
-.tar files won't be preprocessed by a TextFilter
-fixed bug from PAQ8C with small files

PAQ8C (12.02.2006)
-includes TextFilter 2.0 for PAQ by P.Skibinski
-new dictionaries
-major improvements in file type detection routines
-file list sorting depend on type of file

PAQ8B (07.02.2006)
-includes TextFilter 1.2 for PAQ by P.Skibinski (based on WRT 4.5)
-fixed bug with not removed temporary file while compression with a filter
-compiled on Intel9 (thanks to Johan)



4. If you're compressing multi-lingual files you can put additional dictionaries
   into the "TextFilter" directory (they are auto-detected). For now there are
   7 additional dictionaries available: German, French, Spanish, Finnish, Polish,
   Italian, and Russian (available at http://www.ii.uni.wroc.pl/~inikep/research/dicts/)

You can easily add your own dictionaries. Structure of a dictionary is following:
1st line - a number of words in a dictionary (used only to select codewords)
2nd line - original charset, capital letters (for national letters in multi-lingual files)
3rd line - original charset, lower-case letters (for national letters in multi-lingual files)
4th line - second charset, capital letters (different encoding)
5th line - second charset, lower-case letters (different encoding)
6th line - third charset, capital letters (different encoding)
7th line - third charset, lower-case letters (different encoding)
8th line - fourth charset, capital letters (different encoding)
9th line - fourth charset, lower-case letters (different encoding)
10th line - fifth charset, capital letters (different encoding)
11th line - fifth charset, lower-case letters (different encoding)
12th line - sixth charset, capital letters (different encoding)
13th line - sixth charset, lower-case letters (different encoding)
14th line - first word in a dictionary
15th line - second word in a dictionary
16th line - and so on

Example (WRT-PL.DIC):
191796
¥ÆÊ£ÑÓŒ¯ - original charset (Windows Latin2/cp1250)
¹æê³ñóœ¿Ÿ
¡ÆÊ£ÑÓ¦¯¬ - second charset (ISO-8859-2)
±æê³ñó¶¿¼
¤¨ãà—½ - third charset (DOS Latin2/cp852)
¥†©ˆä¢˜¾«
•œ¥£˜¡  - fourth charset (DOS Mazovia)
†‘’¤¢ž§¦
ÂÊËÎÏÓÔÛÚ - fifth charset (AmigaPL)
âêëîïóôûú
ACELNOSZZ - sixth charset (no polish letters)
acelnoszz
na - first word in a dictionary
do - second word in a dictionary
