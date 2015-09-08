PAQ8E (23.02.2006) - File archiver and compressor (based on PAQ8A).
------------------   (C) 2006, M. Mahoney (mmahoney@cs.fit.edu)
                               P. Skibinski (inikep@o2.pl)
                     http://www.ii.uni.wroc.pl/~inikep/research/paq8e.zip


1. Introduction
PAQ8E is PAQ8A with added TextFilter - a preprocessor (WRT by P. Skibinski),
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
To compress: paq8e [-option] archive files...  (archive will be created)
   or (Windows): dir/b | paq8e archive  (file names read from input)
To decompress/compare: paq8e archive [files...] 
To view contents: more < archive

Options are -0 (fast, 18MB) to -9 (slow, 4GB memory, best compression).
The -4 (116 MB) is default, and is used to trade off compression vs. speed
and memory. 



3. History
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
   4 additional dictionaries available: German, French, Polish and Russian:
WRT-DE.DIC, 463918 bytes (available at http://www.ii.uni.wroc.pl/~inikep/research/wrt40-de.rar)
WRT-FR.DIC, 648314 bytes (available at http://www.ii.uni.wroc.pl/~inikep/research/wrt40-fr.rar)
WRT-PL.DIC, 1479847 bytes (available at http://www.ii.uni.wroc.pl/~inikep/research/wrt40-pl.rar)
WRT-RU.DIC, 4954251 bytes (available at http://www.ii.uni.wroc.pl/~inikep/research/wrt40-ru.rar)

2. You can easily add your own dictionaries. Structure of a dictionary is following:
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
