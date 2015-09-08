PAQ8C (12.02.2006)
-includes TextFilter 2.0 for PAQ by P.Skibinski
-new dictionaries
-major improvements in file type detection routines
-file list sorting depend on type of file

PAQ8B (07.02.2006)
-includes TextFilter 1.2 for PAQ by P.Skibinski (based on WRT 4.5)
-fixed bug with not removed temporary file while compression with a filter
-compiled on Intel9 (thanks to Johan)



1. If you're compressing multi-lingual files you can put additional dictionaries
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
†‘’¤¢§¦
ÂÊËÎÏÓÔÛÚ - fifth charset (AmigaPL)
âêëîïóôûú
ACELNOSZZ - sixth charset (no polish letters)
acelnoszz
na - first word in a dictionary
do - second word in a dictionary
