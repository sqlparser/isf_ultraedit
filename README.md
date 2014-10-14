isf_ultraedit
=============

SQL Formatter plugin for UltraEdit


####Install
Just download [sqlformat.exe](https://github.com/sqlparser/isf_ultraedit/blob/master/bin/sqlFormat.exe?raw=true)
and save it into any directory c:\<your sqlFormat dir>\.

Config this plugin via Advanced -> Tool configuration:

Enter <your sqlFormat dir>\sqlFormat.exe -s "%sel%" in Command line

<img src="https://github.com/sqlparser/isf_ultraedit/blob/master/tutor/format_selected_sql_config1.png" alt="sql format plugin for UltraEdit setup 1">

Set output options like this:

<img src="https://github.com/sqlparser/isf_ultraedit/blob/master/tutor/format_selected_sql_config2.png" alt="sql format plugin for UltraEdit setup 2">


You can also download [format option file](https://github.com/sqlparser/isf_ultraedit/blob/master/bin/fo.json)
and put it into c:\<your sqlFormat dir>\.  You can change options in this file to control the layout of formatted sql.

For detailed explanation of those format options, Please check document here:

https://github.com/sqlparser/sql-pretty-printer/wiki/SQL-Coding-Standard-and-Guideline

This sql formatter plugin works under UltraEdit v5.1 and later.

####Usage
Format selected SQL in editor by click "Format selected SQL" via menu advanced -> Format selected SQL,
or use hotkey Ctrl+Shift+0.


####NOTE
In order to use this sql formatter plugin for UltraEdit, you should be online.
This plugin send your SQL to our SQL FaaS server here:

https://github.com/sqlparser/sql-pretty-printer/wiki/SQL-FaaS#2-api

If you don't want to send your SQL via internet, you may conside to use 
[sql pretty printer desktop version](http://www.dpriver.com/?ref=github_isf_notepadd)


####Contact
http://www.dpriver.com

support@dpriver.com



####Format options:
https://github.com/sqlparser/sql-pretty-printer/wiki/SQL-Coding-Standard-and-Guideline

####Links:
[sql pretty printer desktop version](http://www.dpriver.com/products/sqlpp/desktop_index.php)

[sql pretty printer addin for SQL Server Management Studio](http://www.dpriver.com/products/sqlpp/ssms_index.php)

[sql pretty printer addin for Visual Studio](http://www.dpriver.com/products/sqlpp/vs_index.php)

[C client sql formatter](https://github.com/sqlparser/isf_c_client)

[Java client sql formatter](https://github.com/sqlparser/isf_java_client)

[sql formatter plugin for notepad](https://github.com/sqlparser/isf_notepad)

[sql formatter plugin for eclipse](https://github.com/sqlparser/isf_eclipse)
