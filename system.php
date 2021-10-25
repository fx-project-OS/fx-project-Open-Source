<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : system.php                                                   //
// Version     : 21.2                                                         //
// Begin       : 2021-02-10                                                   //
// Last Change : 2021-05-05                                                   //
// Author      : FeRox Management Consulting GmbH & Co. KG                    //
//               Adolf-Langer-Weg 11a, D-94036 Passau (Germany)               //
//               https://www.ferox.de - info@ferox.de                         //
// License     : GNU-GPL v3 (https://opensource.org/licenses/GPL-3.0)         //
// -------------------------------------------------------------------------- //
// fx-project - An open source PHP Project Managament Software                //
// Copyright  © FeRox Management Consulting GmbH & Co. KG                     //
// -------------------------------------------------------------------------- //
// This program is free software: you can redistribute it and/or modify       //
// it under the terms of the GNU General Public License as published by       //
// the Free Software Foundation, either version 3 of the License, or          //
// (at your option) any later version.                                        //
//                                                                            //
// This program is distributed in the hope that it will be useful,            //
// but WITHOUT ANY WARRANTY; without even the implied warranty of             //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              //
// GNU General Public License for more details.                               //
//                                                                            //
// You should have received a copy of the GNU General Public License          //
// along with this program.  If not, see <https://www.gnu.org/licenses/>.     //
//                                                                            //
// See ../LICENSE.TXT file for more information.                              //
// -------------------------------------------------------------------------- //
// LICENSING ADDENDUM:                                                        //
// Programs in the SPP (Special Programs) subfolder are coded extensions of   //
// the open source software fx-project. These programs are offered for sale   //
// by the manufacturer FeRox Management Consulting GmbH & Co. KG and require  //
// a valid key for execution. It is forbidden to resell these programs        //
// and/or keys or to pass them on free of charge or use them without the      //
// express written permission of FeRox Management Consulting GmbH & Co. KG.   //
////////////////////////////////////////////////////////////////////////////////

/**
 * @file
 * System check utilities like:
 * - Info: Display OS, fx-project Version, Timestamp, Server Address, browser according to the browser detection, etc.
 * - PHP Info: Display PHP Info
 * - PHP Extensions: Display mandatory, database and optional found PHP extensions
 * - GD: Display and test GD image creation
 * - ChartDirector: Display and test ChartDirector creation
 * - DB Connection: Display and test database connection ability
 * - DB Checker: Display and test database integrity
 * - Email: Display and test email
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

// Allow call? (0=default=No, 1=yes)
$__system_call=0;

@set_time_limit(0);

// Runtime START
list($usec,$sec)=explode(' ',microtime());
$rts=$sec.substr($usec,1);

// Program filename
$_pfn=ucfirst(substr(basename(__FILE__),0,-4));

// Debug this file? (Uncomment the next line)
//$GLOBALS['__debug'][__FILE__]=1;

// Menu
define('MENU_INFO', 1);
define('MENU_PHP_INFO', 2);
define('MENU_PHP_EXTENSIONS', 3);
define('MENU_GD', 4);
define('MENU_CHARTDIRECTOR', 5);
define('MENU_EMAIL', 6);
define('MENU_DB_CONNECTION', 7);
define('MENU_DB_CHECKER', 8);

$menu_array=array(
	MENU_INFO=>'Info',
	MENU_PHP_INFO=>'PHP Info',
	MENU_PHP_EXTENSIONS=>'PHP Extensions',
	MENU_GD=>'GD',
	MENU_CHARTDIRECTOR=>'ChartDirector',
	MENU_EMAIL=>'Email',
	MENU_DB_CONNECTION=>'DB Connection',
	MENU_DB_CHECKER=>'DB Checker'
);
$menu_selected=-1;
if(isset($_POST['menu_set']))
	$menu_selected=(int)$_POST['menu_set'];
else
{
	foreach($menu_array as $menu_id => $menu_text)
	{
		if(isset($_POST['menu_id_'.$menu_id]))
		{
			$menu_selected=$menu_id;
			break;
		}
	}
}
$mwa_style='background:transparent;';
if($menu_selected > 0)
	$mwa_style='background:#fefefe;box-shadow:2px 2px 4px inset rgba(0,0,0, 0.66);';

// Is it a status check or a result call?
$GLOBALS['__statusfilename']='_'.$_pfn.'_info.fst';
$scheck=false;
$fresult=false;
if(isset($_POST) && is_array($_POST) && sizeof($_POST) && isset($_POST['check_status']))
	$scheck=true;
else if(isset($_POST) && is_array($_POST) && sizeof($_POST) && isset($_POST['fetch_result']))
	$fresult=true;
// First program call, send these additional headers
else if($menu_selected < 0)
{
	$__headers=array(
		'Last-Modified: '.gmdate('D, d M Y H:i:s').' GMT',	// current date, because it is created differently ever time dynamically
		'Cache-Control: no-cache, must-revalidate',  		// HTTP/1.1
		'Pragma: no-cache'                         			// HTTP/1.0
	);
}

// Search for and include "basics.inc" to set all definitions, variables and necessary dynamic paths
$__pnm='basics.inc';
if(!isset($GLOBALS['__loaded_'.$__pnm]))
{
	$__prg=$__pnm;
	$__pfc=0;
	while(($__pfc < 10) && !file_exists($__prg))
	{
		$__pfc++;
		$__prg='../'.$__prg;
	}
	if(file_exists($__prg))
		require($__prg);
	else
		die('<div style="padding:8px;"><h1 style="padding:8px;color:red;background:#fff;border:2px solid red;box-shadow:4px 4px 4px rgba(0,0,0, 0.5);">fx-project - ERROR: Necessary file &quot;'.$__pnm.'&quot; not found!</h1></div>');
}

// Call prohibited?
if(!$__system_call)
{
	$_title='Notice';
	$_message='>System call not allowed!';
	if($GLOBALS['fxpglobals']['lang'] == 1)	// Deutsch
	{
		$_title='Hinweis';
		$_message='>Systemaufruf nicht erlaubt!';
	}

	require($GLOBALS['mainpath'].'error.inc');
	die;
}

// Status check call?
if($scheck)
{
	$status=fxProgressStatus();
	die($status);
}

// Fetch result call?
if($fresult)
{
	$pa=array('prefix', 'name', 'cls');
	foreach($pa as $pan)
	{
		$GLOBALS[$pan]='';
		if(isset($_POST) && is_array($_POST) && sizeof($_POST) && isset($_POST[$pan]))
			$GLOBALS[$pan]=trim($_POST[$pan]);
	}
}

// First program call, display html frame
$html_head='';
if($menu_selected < 0)
{
	// HTML head
	$html_head  = '<head>'.$GLOBALS['nl'];
	$html_head .= '	<title>fx-project '.$_pfn.'</title>'.$GLOBALS['nl'];
	$html_head .= '	<meta http-equiv="content-type" content="text/html; charset='.FXP_CHARSET.'">'.$GLOBALS['nl'];
	$html_head .= '	<link rel="shortcut icon" href="'.fxGetFavicon().'">'.$GLOBALS['nl'];
	$html_head .= '	<link rel="stylesheet" type="text/css" href="'.$GLOBALS['csspath'].'00default.css">'.$GLOBALS['nl'];
	$html_head .= '	<style type="text/css">'.$GLOBALS['nl'];
	$html_head .= '		.mns, .mnas, .mnls, .mni {font-size:12pt;border:0;padding:0;margin:0;background:transparent;}'.$GLOBALS['nl'];
	$html_head .= '		.mns {color:rgb(255,255,255, 0.6);}'.$GLOBALS['nl'];
	$html_head .= '		.mns:hover {color:rgb(255,255,255, 0.85);}'.$GLOBALS['nl'];
	$html_head .= '		.mnas {background:linear-gradient(to bottom,rgba(255,255,255, 0.5),rgba(0,0,0, 0.5));color:rgb(255,255,255, 0.85);text-shadow:2px 2px 2px rgba(0,0,0, 0.66);}'.$GLOBALS['nl'];
	$html_head .= '		.mnas:hover {color:rgb(255,255,255, 1.0);}'.$GLOBALS['nl'];
	$html_head .= '		.mnls {color:#006b9f;font-weight:bold;}'.$GLOBALS['nl'];
	$html_head .= '		.mnls:hover {color:#3d8cb3;font-weight:bold;}'.$GLOBALS['nl'];
	$html_head .= '		.mni {color:rgba(255,255,255, 0.1);}'.$GLOBALS['nl'];
	$html_head .= '	</style>'.$GLOBALS['nl'];
	$html_head .= '</head>'.$GLOBALS['nl'];

	// HTML body
	$html_body  = '<body class="fxbody">'.$GLOBALS['nl'];
	// ...Info passed to javascript: program name | datetime | local session key
	$html_body .= '	<div id="info" style="display:none;">'.basename(__FILE__).'|'.$GLOBALS['datetime'].'|'.$GLOBALS['locseskey'].'</div>'.$GLOBALS['nl'];
	// ...Status
	$html_body .= '	<div id="status" style="position:absolute;left:0;top:0;right:0;bottom:0;display:none;z-index:2000;">'.$GLOBALS['nl'];
	$html_body .= '		<div style="position:absolute;left:0;top:0;right:0;bottom:0;background:#000;opacity:0.5;"></div>'.$GLOBALS['nl'];
	$html_body .= '		<div id="statusd" style="position:absolute;top:33%;left:12%;width:76%;border:1px solid black;border-radius:8px;height:20px;margin-bottom:64px;background:#00344e;box-shadow:8px 8px 8px rgba(0,0,0, 0.5);display:none;">'.$GLOBALS['nl'];
	$html_body .= '			<div id="statush" style="position:absolute;top:-48px;left:0;font-size:18pt;color:#fff;text-shadow:4px 4px 4px rgba(0,0,0, 0.5);"></div>'.$GLOBALS['nl'];
	$html_body .= '			<div id="statusb" style="position:absolute;left:0;top:0;width:0%;border:0;border-radius:8px;height:20px;background:#fff;"></div>'.$GLOBALS['nl'];
	$html_body .= '			<div id="statust" style="position:absolute;top:-2px;width:100%;font-size:14pt;text-align:center;color:#006b9f;"></div>'.$GLOBALS['nl'];
	$html_body .= '		</div>'.$GLOBALS['nl'];
	$html_body .= '		<div id="statusm" style="position:absolute;top:40%;left:0;right:0;font-size:22pt;color:#fff;text-align:center;text-shadow:4px 4px 4px rgba(0,0,0, 0.5);"></div>'.$GLOBALS['nl'];
	$html_body .= '		<div style="position:absolute;left:0;top:48%;right:0;bottom:0;"><div class="wacircle1"></div><div class="wacircle2"></div></div>'.$GLOBALS['nl'];
	$html_body .= '	</div>'.$GLOBALS['nl'];
	// ...Title bar
	$html_body .= '	<div class="fxtoolc nousersel" style="position:fixed;left:0;top:0;right:0;height:47px;z-index:150;">'.$GLOBALS['nl'];
	$html_body .= '		<img id="fxswlogobg" src="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAFkAAAAvCAYAAACIcGZzAAABA0lEQVR4nO3QsQ1CQRAD0emJNqiDNmidgPATILizfdauNPn6ATyn5T2AO++7EfBQWxfgQRYAD7IAeJAFwIMsAB5kAfAgC4AHWQA8yALgQRYAD7IAeJAFwAQMSO9vYAJGJLcEmIAhqS0DJmBMYkuBCRiU1nJgAkYltQWYgGEpbQMmYFxCW4EJGOhuOzABI+uBCRhaD4xhXEJSYASD0pIDs2FEchZgFjx+SjZgfnj2xKzAfPHg6dmB+fBUUxHA4IeoBwY/Rj0w+EHqgcGPUg8Mfph6YPDj1AODH6geGPxI9cDgh6oHBj9WPTD4weqBwY9WDwx+uHpg8OPVA4MfsB4Y/Ij1wC+9Mc/1UdaqWQAAAABJRU5ErkJggg==">'.$GLOBALS['nl'];
	$html_body .= '		'.($GLOBALS['_elinks']?'<a href="https://www.fx-project.org" target="_blank">':'').'<img id="fxswlogo" src="'.fxGetLogo('small').'" tooltip="fx-project '.$v_type.' V '.FXP_VERSION.'">'.($GLOBALS['_elinks']?'</a>':'').$GLOBALS['nl'];
	$html_body .= '		<div style="position:absolute;left:88px;top:4px;text-shadow:2px 2px 2px rgba(0,0,0, 0.5);"><span style="color:#fff;font-size:22pt;">f</span><span style="color:#e1001a;font-size:20pt;">x</span><span style="color:#001a26;font-size:18pt;">-project</span></div>'.$GLOBALS['nl'];
	$html_body .= '		<div style="position:absolute;right:8px;top:9px;color:#ffffff;font-size:18pt;font-weight:bolder;text-shadow:2px 2px 2px rgba(0,0,0, 0.5);">'.$_pfn.'</div>'.$GLOBALS['nl'];
	$html_body .= '	</div>'.$GLOBALS['nl'];
	// ...Work area (with runtime)
	$html_body .= '	<div id="work" style="position:fixed;left:0;top:48px;right:0;bottom:0;">'.$GLOBALS['nl'];
	$html_body .= '		<div id="runtime" style="position:fixed;right:4px;bottom:2px;color:rgb(0,0,0, 0.9);font-size:5pt;"><!--#RUNTIME#--></div>'.$GLOBALS['nl'];
	$html_body .= '	</div>'.$GLOBALS['nl'];
	// ...JavaScript
	$html_body .= '	<script src="'.$GLOBALS['jscpath'].'prototype.js"></script> <script src="'.$GLOBALS['jscpath'].'scriptaculous.js"></script> <script src="'.$GLOBALS['jscpath'].'int.js"></script>'.$GLOBALS['nl'];
	$html_body .= '</body>'.$GLOBALS['nl'];
}
else
{
	// Display styles
	$ws='font-size:14pt;font-weight:bold;';
	$wbs='<b style="'.$ws.'color:#006b9f;">';

	// Screen outputs
	$whtml='';

	// Do we have Zip Extension?
	$GLOBALS['zip_ext']=false;
	if(extension_loaded('zip'))
		$GLOBALS['zip_ext']=true;

	// We are Sysadmin
	$GLOBALS['fxpglobals']['client']=0;
	$GLOBALS['fxpglobals']['user']=1;
	$GLOBALS['fxpglobals']['person']=0;
	$GLOBALS['fxpglobals']['lang']=7;

	// Extended mode if submitted key matches
	$em=false;
	if(fxIsArray($GLOBALS['fxpglobals']['dbparam']) && isset($GLOBALS['fxpglobals']['dbparam']['oskey32']) && isset($GLOBALS['fxpglobals']['dbparam']['oskey64']))
	{
		$k16=substr($GLOBALS['fxpglobals']['dbparam']['oskey32'],0,8).substr($GLOBALS['fxpglobals']['dbparam']['oskey64'],0,8);
		if(($k16 === $emode) && (strlen($k16) == 16))
			$em=true;
//fxDebug($em,'$em: $emode='.$emode.', $k16='.$k16, 0);
	}

	// Do we have db informations and can we open the db?
	ob_start();
	$GLOBALS['dbid']=false;
	$dberror=@db_ini(true);
	$GLOBALS['odbid']=$GLOBALS['dbid'];
	ob_end_clean();

	ob_start();
	$ok='<div style="display:inline;padding:2px 8px;color:#009f6b;font-weight:bolder;border:1px solid #009f6b;border-radius:50%;">OK</div>';
	$ko='<div style="display:inline;padding:2px 8px;color:#e1001a;font-weight:bolder;border:1px solid #e1001a;border-radius:50%;">KO</div>';
	$er='<div style="display:inline;padding:2px 8px;color:#e1001a;font-weight:bolder;border:1px solid #e1001a;border-radius:50%;">ERROR</div>';
	$sk='<div style="display:inline;padding:2px 8px;color:#888;font-weight:bolder;border:1px solid #888;border-radius:50%;">SKIP</div>';
	$in='<div style="display:inline;padding:2px 8px;color:#888;font-weight:bolder;border:1px solid #888;border-radius:50%;">INFO</div>';

	switch($menu_selected)
	{
		case MENU_INFO:
			$browser_full=strtolower($_SERVER['HTTP_USER_AGENT']);
			$browser_text=fxf_detBrowser();

			$sadr=$_SERVER['SERVER_ADDR'];
			$snam=strtolower($_SERVER['SERVER_NAME']);
			$sadm=strtolower($_SERVER['SERVER_ADMIN']);

			echo('<table width="100%" cellpadding="2" cellspacing="0">');
			echo('	<tr><td colspan="2"><div class="s4b" style="padding:2px 8px;background:#ddd;border-bottom:1px solid #000;">fx-project:</div></td></tr>'.$GLOBALS['nl']);
			echo('	<tr><td valign="top" nowrap><span class="darkgrey">Version:</span>&nbsp;</td><td width="100%">'.$v_type.' <font class="darkgrey">V '.FXP_VERSION.'</font></td></tr>');

			echo('	<tr><td colspan="2"><br /><div class="s4b" style="padding:2px 8px;background:#ddd;border-bottom:1px solid #000;">Environment:</div></td></tr>'.$GLOBALS['nl']);
			echo('	<tr><td valign="top" nowrap><span class="darkgrey">Server OS:</span>&nbsp;</td><td>'.$GLOBALS['_os'].'<br /></td></tr>');
			echo('	<tr><td valign="top" nowrap><span class="darkgrey">Charset:</span>&nbsp;</td><td><span class="grey">[INI] '.$ini_charset.'</span> <b class="darkgrey">&rarr;</b> [FXP] '.$fxp_charset.'</td></tr>');
			echo('	<tr><td valign="top" nowrap><span class="darkgrey">Timestamp:</span>&nbsp;</td><td>'.$GLOBALS['datetime'].' <span class="grey">(= '.get_entry($GLOBALS['datetime'], 'datetime').')</span></td></tr>');

			echo('	<tr><td colspan="2"><br /><div class="s4b" style="padding:2px 8px;background:#ddd;border-bottom:1px solid #000;">Browser:</div></td></tr>'.$GLOBALS['nl']);
			echo('	<tr><td valign="top" nowrap><span class="darkgrey">HTTP_USER_AGENT:</span>&nbsp;</td><td>'.$browser_full.'<br /><b>&rarr; '.$browser_text.'</b></td></tr>');

			echo('	<tr><td colspan="2"><br /><div class="s4b" style="padding:2px 8px;background:#ddd;border-bottom:1px solid #000;">Storage:</div></td></tr>'.$GLOBALS['nl']);
			echo('	<tr><td valign="top" nowrap><span class="darkgrey">Local Storage ID:</span>&nbsp;</td><td>'.$GLOBALS['fxpglobals']['locstoid'].'</td></tr>');
			if($em)
				echo('	<tr><td valign="top" nowrap><span class="darkgrey">Session Storage ID:</span>&nbsp;</td><td>'.$GLOBALS['fxpglobals']['sesstoid'].'</td></tr>');
			echo('	<tr><td valign="top" nowrap><span class="darkgrey">Path:</span>&nbsp;</td><td>'.$GLOBALS['mainpath'].'</td></tr>');

			if($em)
			{
			echo('	<tr><td colspan="2"><br /><div class="s4b" style="padding:2px 8px;background:#ddd;border-bottom:1px solid #000;">Server:</div></td></tr>'.$GLOBALS['nl']);
				echo('	<tr><td valign="top" nowrap><span class="darkgrey">SERVER_ADDR:</span>&nbsp;</td><td>'.$sadr.'</td></tr>');
				echo('	<tr><td valign="top" nowrap><span class="darkgrey">SERVER_NAME:</span>&nbsp;</td><td>'.$snam.'</td></tr>');
				echo('	<tr><td valign="top" nowrap><span class="darkgrey">SERVER_ADMIN:</span>&nbsp;</td><td>'.$sadm.'</td></tr>');
			}
			echo('</table>');
		break;

		case MENU_PHP_INFO:
			$vers=phpversion();
			$varr=explode('.',$vers);
			foreach($varr as $vl => $vn)
				$varr[$vl]=(int)$vn;
//fxDebug($varr,'$vers='.$vers, 0);

			ob_end_clean();

			ob_start();
			phpinfo();
			$pic=ob_get_contents();
			ob_end_clean();

			ob_start();

			$bp=strpos($pic,'<body>');
			$pic=trim(substr($pic,$bp+6));

			$bp=strpos($pic,'</body>');
			$pic=trim(substr($pic,0,$bp));

			if(substr($pic,0,4) == '<div')
			{
				$nwidth=934;

				$edp=strpos($pic,'>');
				$pic=trim(substr($pic,$edp+1));
				$pic=trim(substr($pic,0,-6));

				$ps  = '<style type="text/css">'.$GLOBALS['nl'];
				$ps .= 'pre {margin:0;font-family:monospace;}'.$GLOBALS['nl'];
				$ps .= '	table {border-collapse:collapse;border:0;width:'.$nwidth.'px;box-shadow:1px 2px 3px #cccccc;}'.$GLOBALS['nl'];
				$ps .= '	.center {text-align:center;}'.$GLOBALS['nl'];
				$ps .= '	.center table {margin:1em auto;text-align:left;}'.$GLOBALS['nl'];
				$ps .= '	.center th {text-align:center !important;}'.$GLOBALS['nl'];
				$ps .= '	td, th {border:1px solid #666666;font-size:100%;vertical-align:baseline;padding:4px 5px;}'.$GLOBALS['nl'];
				$ps .= '	h1 {font-size:150%;}'.$GLOBALS['nl'];
				$ps .= '	h2 {font-size:125%;}'.$GLOBALS['nl'];
				$ps .= '	.p {text-align:left;}'.$GLOBALS['nl'];
				$ps .= '	.e {background-color:#e8e8e8;width:300px;font-weight:bold;color:#000000;}'.$GLOBALS['nl'];
				$ps .= '	.h {background-color:#a8a8a8;font-weight:bold;color:#000000;}'.$GLOBALS['nl'];
				$ps .= '	.v {background-color:#f8f8f8;max-width:300px;overflow-x:auto;color:#000000;}'.$GLOBALS['nl'];
				$ps .= '	.v i {color:#999999;}'.$GLOBALS['nl'];
				$ps .= '	img {float:right; border:0;}'.$GLOBALS['nl'];
				$ps .= '	hr {width:'.$nwidth.'px;background-color:#cccccc;border:0;height:1px;color:#000000;}'.$GLOBALS['nl'];
				$ps .= '</style>'.$GLOBALS['nl'];
			}
			else
			{
				$nwidth=800;
				$pic=str_replace('600', $nwidth, $pic);

				$ps  = '<style type="text/css">'.$GLOBALS['nl'];
				$ps .= '	pre {margin:0;font-family:monospace;}'.$GLOBALS['nl'];
				$ps .= '	h2 {font-size:200%;color:#006b9f;}'.$GLOBALS['nl'];
				$ps .= '	table {border-collapse:collapse;}'.$GLOBALS['nl'];
				$ps .= '	td, th {border:1px solid #666666;vertical-align:baseline;}'.$GLOBALS['nl'];
				$ps .= '	.center th {text-align:center !important;}'.$GLOBALS['nl'];
				$ps .= '	.p {text-align:left;}'.$GLOBALS['nl'];
				$ps .= '	.e {background-color:#e8e8e8;font-weight:bold;color:#000000;}'.$GLOBALS['nl'];
				$ps .= '	.h {background-color:#a8a8a8;font-weight:bold;color:#000000;}'.$GLOBALS['nl'];
				$ps .= '	.v {background-color:#f8f8f8;color:#000000;}'.$GLOBALS['nl'];
				$ps .= '	.vr {background-color:#e8e8e8;text-align:right;color:#000000;}'.$GLOBALS['nl'];
				$ps .= '	img {float:right;border:0;}'.$GLOBALS['nl'];
				$ps .= '	hr {width:'.$nwidth.'px;background-color:#cccccc;border:0;height:1px;color:#000000;}'.$GLOBALS['nl'];
				$ps .= '	</style>'.$GLOBALS['nl'];
			}

			if(!$em)
			{
				$etp=stripos($pic,'</table>');
				if($etp)
					$pic=trim(substr($pic,0,$etp+8));
			}

			echo($ps.$pic);
		break;

		case MENU_PHP_EXTENSIONS:
			$dbt=0;
			if(fxIsArray($GLOBALS['fxpglobals']['dbparam']) && isset($GLOBALS['fxpglobals']['dbparam']['dbtype']))
				$dbt=$GLOBALS['fxpglobals']['dbparam']['dbtype'];
//fxDebug($dbt,'$dbt', 0);

			$ea=fxf_checkExtensions($dbt);
//fxDebug($ea,'$ea', 0);

			echo('<div style="line-height:26px;">'.$GLOBALS['nl']);

			// ...Specified Database Type
			echo('	<div class="s4b" style="padding:2px 8px;background:#ddd;border-bottom:1px solid #000;">Specified Database Type:</div>'.$GLOBALS['nl']);
			echo('	&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;');
			if($dbt && fxIsArray($GLOBALS['_dbtypes']) && isset($GLOBALS['_dbtypes'][$dbt]))
				echo($ok.'&nbsp;&nbsp;<b>'.$dbt.':&nbsp;'.$GLOBALS['_dbtypes'][$dbt]['type'].'</b>&nbsp;&nbsp;<span>(=&nbsp;'.$GLOBALS['_dbtypes'][$dbt]['text'].')</span>');
			else
			{
				$adt='';
				foreach($GLOBALS['_dbtypes'] as $dbti => $dbta)
				{
					if(strlen($adt))
						$adt .= ',&nbsp;&nbsp;';
					$adt .= '<b>'.$dbti.'</b>:&nbsp;'.$dbta['type'].'&nbsp;&nbsp;(=&nbsp;'.$dbta['text'].')';
				}
				echo($sk.'&nbsp;&nbsp;<span class="grey">No type specified!&nbsp;&nbsp;Available types are: '.$adt.'</span>');
			}
			echo('<br style="clear:both;" /><br />'.$GLOBALS['nl']);

			// ...OK
			echo('	<div class="s4b" style="padding:2px 8px;background:#ddd;border-bottom:1px solid #000;">Extensions OK:</div>'.$GLOBALS['nl']);
			if(strlen($ea['ok']))
			{
				$eaa=explode(',', $ea['ok']);
				foreach($eaa as $en)
				{
					$en=substr(trim($en),1,-1);
					echo('	&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;'.$ok.'&nbsp;&nbsp;<b>'.$en.'</b><br style="clear:both;" />'.$GLOBALS['nl']);
				}
			}
			else
				echo('	&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;'.$sk.'&nbsp;&nbsp;<span class="grey">No extensions found</span><br style="clear:both;" />'.$GLOBALS['nl']);
			echo('	<br />'.$GLOBALS['nl']);

			// ...Not found
			echo('	<div class="s4b" style="padding:2px 8px;background:#ddd;border-bottom:1px solid #000;">Optional Missing Extensions:</div>'.$GLOBALS['nl']);
			if(strlen($ea['notfound']))
			{
				$eaa=explode(',', $ea['notfound']);
				foreach($eaa as $en)
				{
					$en=substr(trim($en),1,-1);
					echo('	&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;'.$sk.'&nbsp;&nbsp;<b>'.$en.'</b><br style="clear:both;" />'.$GLOBALS['nl']);
				}
			}
			else
				echo('	&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;'.$sk.'&nbsp;&nbsp;<span class="grey">No extensions found</span><br style="clear:both;" />'.$GLOBALS['nl']);
			echo('	<br />'.$GLOBALS['nl']);

			// ...Missing
			echo('	<div class="s4b" style="padding:2px 8px;background:#ddd;border-bottom:1px solid #000;">Mandatory Missing Extensions:</div>'.$GLOBALS['nl']);
			if(strlen($ea['missing']))
			{
				$eaa=explode(',', $ea['missing']);
				foreach($eaa as $en)
				{
					$en=substr(trim($en),1,-1);
					echo('	&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;'.$er.'&nbsp;&nbsp;<b>'.$en.'</b><br style="clear:both;" />'.$GLOBALS['nl']);
				}
			}
			else
				echo('	&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;'.$sk.'&nbsp;&nbsp;<span class="grey">No extensions found</span><br style="clear:both;" />'.$GLOBALS['nl']);
			echo('	<br />'.$GLOBALS['nl']);

			echo('</div>'.$GLOBALS['nl']);
		break;

		case MENU_GD:
			$ea=fxf_checkExtensions($dbt);
//fxDebug($ea,'$ea', 0);

			$iw=256;
			$ih=128;

			$sia=array('png'=>'png','gif'=>'png','jpg'=>'jpeg');
			foreach($sia as $sit => $gds)
			{
				${'filename_'.$sit}=$GLOBALS['tmppath'].'~gd_test.'.$sit;
				if(file_exists(${'filename_'.$sit}))
					unlink(${'filename_'.$sit});
			}

			echo('<div style="line-height:26px;">'.$GLOBALS['nl']);

			// ...Checking Extension
			echo('	<div class="s4b" style="padding:2px 8px;background:#ddd;border-bottom:1px solid #000;">Checking Extension:</div>'.$GLOBALS['nl']);
			if(fxIsArray($ea['extstat']) && ((isset($ea['extstat']['gd']) && $ea['extstat']['gd']) || (isset($ea['extstat']['gd2']) && $ea['extstat']['gd2'])))
			{
				$en='';
				if(isset($ea['extstat']['gd']) && $ea['extstat']['gd'])
					$en .= 'gd';
				if(isset($ea['extstat']['gd2']) && $ea['extstat']['gd2'])
				{
					if(strlen($en))
						$en .= ', ';
					$en .= 'gd2';
				}
				echo('	&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;'.$ok.'&nbsp;&nbsp;<b>'.$en.'</b><br style="clear:both;" /><br />'.$GLOBALS['nl']);

				// ...Testing GD Functions
				echo('	<div class="s4b" style="padding:2px 8px;background:#ddd;border-bottom:1px solid #000;">Testing GD Functions:</div>'.$GLOBALS['nl']);
				$gfn='imagecreatetruecolor';
				echo('	&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;Creating true color image '.$iw.'x'.$ih.'&nbsp;&nbsp;<font class="s1 lightgrey">...</font>&nbsp;&nbsp;');
				if(function_exists($gfn))
				{
					$im=imagecreatetruecolor($iw,$ih);
					if($im)
					{
						echo($ok.'&nbsp;&nbsp;<span class="green">GD function &quot;'.$gfn.'&quot;: Image could be created.</span><br style="clear:both;" />'.$GLOBALS['nl']);

						// Alpha blending?
						$gfn='imagealphablending';
						echo('<br />&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;Use alpha blending?&nbsp;&nbsp;<font class="s1 lightgrey">...</font>&nbsp;&nbsp;');
						if(function_exists($gfn))
						{
							imagealphablending($im, true);
							echo($ok.'&nbsp;&nbsp;<span class="green">GD function &quot;'.$gfn.'&quot;: Alpha blending available and used.</span><br style="clear:both;" />'.$GLOBALS['nl']);
						}
						else
							echo($sk.'&nbsp;&nbsp;<span class="grey">GD function &quot;'.$gfn.'&quot; does not exist!</span><br style="clear:both;" />'.$GLOBALS['nl']);

						// Image antialias?
						$gfn='imageantialias';
						echo('&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;Use image antialias?&nbsp;&nbsp;<font class="s1 lightgrey">...</font>&nbsp;&nbsp;');
						if(function_exists($gfn))
						{
							imageantialias($im, true);
							echo($ok.'&nbsp;&nbsp;<span class="green">GD function &quot;'.$gfn.'&quot;: Image antialias available and used.</span><br style="clear:both;" />'.$GLOBALS['nl']);
						}
						else
							echo($sk.'&nbsp;&nbsp;<span class="grey">GD function &quot;'.$gfn.'&quot; does not exist!</span><br style="clear:both;" />'.$GLOBALS['nl']);

						// Allocate colors
						$ca=array(
							'white'	=> array(255,255,255),
							'black'	=> array(0,0,0),
							'red'	=> array(225,0,26),
							'green'	=> array(0,159,107),
							'blue'	=> array(0,107,159)
						);
						foreach($ca as $cn => $rgb)
							${'col_'.$cn} = 0;
						$gfn='imagecolorallocate';
						echo('<br />&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;Allocate colors&nbsp;&nbsp;<font class="s1 lightgrey">...</font>&nbsp;&nbsp;');
						if(function_exists($gfn))
						{
							echo($ok.'&nbsp;&nbsp;<span class="green">GD function &quot;'.$gfn.'&quot;: Allocating colors is available.</span><br style="clear:both;" />'.$GLOBALS['nl']);
							foreach($ca as $cn => $rgb)
							{
								${'col_'.$cn} = imagecolorallocate($im, $rgb[0], $rgb[1], $rgb[2]);
								echo('&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&middot;&nbsp;&nbsp;Allocate color &quot;'.$cn.'&quot; (r='.$rgb[0].', g='.$rgb[1].', b='.$rgb[2].')&nbsp;&nbsp;<font class="s1 lightgrey">...</font>&nbsp;&nbsp;<span class="green">Result='.${'col_'.$cn}.'</span><br style="clear:both;" />'.$GLOBALS['nl']);
							}
						}
						else
							echo($er.'&nbsp;&nbsp;<span class="red">GD function &quot;'.$gfn.'&quot; does not exist!</span><br style="clear:both;" />'.$GLOBALS['nl']);

						// Allocate alpha color
						$col_grey=0;
						$gfn='imagecolorallocatealpha';
						echo('<br />&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;Allocate 75% alpha color &quot;grey&quot; (r=180, g=180, b=180)&nbsp;&nbsp;<font class="s1 lightgrey">...</font>&nbsp;&nbsp;');
						if(function_exists($gfn))
						{
							$col_grey = imagecolorallocatealpha($im, 180, 180, 180, 75);
							echo($ok.'&nbsp;&nbsp;<span class="green">GD function &quot;'.$gfn.'&quot;: Allocating alpha colors is available. Result='.$col_grey.'</span><br style="clear:both;" />'.$GLOBALS['nl']);
						}
						else
							echo($er.'&nbsp;&nbsp;<span class="red">GD function &quot;'.$gfn.'&quot; does not exist!</span><br style="clear:both;" />'.$GLOBALS['nl']);

						// Filled rectangle
						$gfn='imagefilledrectangle';
						echo('<br />&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;Draw filled rectangle with &quot;white&quot; background color&nbsp;&nbsp;<font class="s1 lightgrey">...</font>&nbsp;&nbsp;');
						if(function_exists($gfn))
						{
							$gok=imagefilledrectangle($im, 0,0, $iw-1,$ih-1, $col_white);
							if($gok)
								echo($ok.'&nbsp;&nbsp;<span class="green">GD function &quot;'.$gfn.'&quot;: Filled rectangle drawn.</span><br style="clear:both;" />'.$GLOBALS['nl']);
							else
								echo($er.'&nbsp;&nbsp;<span class="red">GD function &quot;'.$gfn.'&quot;: Filled rectangle could not be drawn!</span><br style="clear:both;" />'.$GLOBALS['nl']);
						}
						else
							echo($er.'&nbsp;&nbsp;<span class="red">GD function &quot;'.$gfn.'&quot; does not exist!</span><br style="clear:both;" />'.$GLOBALS['nl']);

						// Rectangle
						$gfn='imagerectangle';
						echo('&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;Draw &quot;black&quot; rectangle as frame&nbsp;&nbsp;<font class="s1 lightgrey">...</font>&nbsp;&nbsp;');
						if(function_exists($gfn))
						{
							$gok=imagerectangle($im, 0,0, $iw-1,$ih-1, $col_black);
							if($gok)
								echo($ok.'&nbsp;&nbsp;<span class="green">GD function &quot;'.$gfn.'&quot;: Rectangle drawn.</span><br style="clear:both;" />'.$GLOBALS['nl']);
							else
								echo($er.'&nbsp;&nbsp;<span class="red">GD function &quot;'.$gfn.'&quot;: Rectangle could not be drawn!</span><br style="clear:both;" />'.$GLOBALS['nl']);
						}
						else
							echo($er.'&nbsp;&nbsp;<span class="red">GD function &quot;'.$gfn.'&quot; does not exist!</span><br style="clear:both;" />'.$GLOBALS['nl']);

						// Lines
						$gfn='imageline';
						echo('&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;Draw 3 horizontal colored lines (&quot;red&quot;, &quot;green&quot; and &quot;blue&quot;)&nbsp;&nbsp;<font class="s1 lightgrey">...</font>&nbsp;&nbsp;');
						if(function_exists($gfn))
						{
							$ihh1=floor($ih/4);
							$gok=imageline($im, 1,$ihh1, $iw-2,$ihh1, $col_red);
							$ihh2=floor($ih/2);
							if($gok)
								$gok=imageline($im, 1,$ihh2, $iw-2,$ihh2, $col_green);
							$ihh3=floor($ih*3/4);
							if($gok)
								$gok=imageline($im, 1,$ihh3, $iw-2,$ihh3, $col_blue);
							if($gok)
								echo($ok.'&nbsp;&nbsp;<span class="green">GD function &quot;'.$gfn.'&quot;: Lines drawn.</span><br style="clear:both;" />'.$GLOBALS['nl']);
							else
								echo($er.'&nbsp;&nbsp;<span class="red">GD function &quot;'.$gfn.'&quot;: Lines could not be drawn!</span><br style="clear:both;" />'.$GLOBALS['nl']);
						}
						else
							echo($er.'&nbsp;&nbsp;<span class="red">GD function &quot;'.$gfn.'&quot; does not exist!</span><br style="clear:both;" />'.$GLOBALS['nl']);

						// Filled ellipse
						$gfn='imagefilledellipse';
						echo('&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;Draw centered &quot;grey&quot; filled alpha ellipse&nbsp;&nbsp;<font class="s1 lightgrey">...</font>&nbsp;&nbsp;');
						if(function_exists($gfn))
						{
							$gok=imagefilledellipse($im, floor($iw/2),floor($ih/2), floor($iw*3/4),floor($ih*3/4), $col_grey);
							if($gok)
								echo($ok.'&nbsp;&nbsp;<span class="green">GD function &quot;'.$gfn.'&quot;: Filled ellipse drawn.</span><br style="clear:both;" />'.$GLOBALS['nl']);
							else
								echo($er.'&nbsp;&nbsp;<span class="red">GD function &quot;'.$gfn.'&quot;: Filled ellipse could not be drawn!</span><br style="clear:both;" />'.$GLOBALS['nl']);
						}
						else
							echo($er.'&nbsp;&nbsp;<span class="red">GD function &quot;'.$gfn.'&quot; does not exist!</span><br style="clear:both;" />'.$GLOBALS['nl']);

						// String
						$str='fx-project GD test';
						$gfn='imagestring';
						echo('&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;Write string &quot;'.$str.'&quot; in &quot;black&quot; onto image&nbsp;&nbsp;<font class="s1 lightgrey">...</font>&nbsp;&nbsp;');
						if(function_exists($gfn))
						{
							$gok=imagestring($im, 5, 4,2, $str, $col_black);
							if($gok)
								echo($ok.'&nbsp;&nbsp;<span class="green">GD function &quot;'.$gfn.'&quot;: String written.</span><br style="clear:both;" />'.$GLOBALS['nl']);
							else
								echo($er.'&nbsp;&nbsp;<span class="red">GD function &quot;'.$gfn.'&quot;: String could not be drawn!</span><br style="clear:both;" />'.$GLOBALS['nl']);
						}
						else
							echo($er.'&nbsp;&nbsp;<span class="red">GD function &quot;'.$gfn.'&quot; does not exist!</span><br style="clear:both;" />'.$GLOBALS['nl']);

						// Save
						echo('<br />');
						foreach($sia as $sit => $gds)
						{
							$gfn='image'.$gds;
							echo('&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;Create '.strtoupper($sit).' &quot;'.${'filename_'.$sit}.'&quot; from temporary gd image&nbsp;&nbsp;<font class="s1 lightgrey">...</font>&nbsp;&nbsp;');
							if(function_exists($gfn))
							{
								if($sit == 'png')
									$gok=imagepng($im,${'filename_'.$sit});
								else if($sit == 'gif')
									$gok=imagegif($im,${'filename_'.$sit});
								else
									$gok=imagejpeg($im,${'filename_'.$sit});

								if($gok)
									echo($ok.'&nbsp;&nbsp;<span class="green">GD function &quot;'.$gfn.'&quot;: '.strtoupper($sit).' created.</span><br style="clear:both;" />'.$GLOBALS['nl']);
								else
									echo($er.'&nbsp;&nbsp;<span class="red">GD function &quot;'.$gfn.'&quot;: '.strtoupper($sit).' could not be created!</span><br style="clear:both;" />'.$GLOBALS['nl']);
							}
							else
								echo($er.'&nbsp;&nbsp;<span class="red">GD function &quot;'.$gfn.'&quot; does not exist!</span><br style="clear:both;" />'.$GLOBALS['nl']);
						}

						// Destroy image
						$gfn='imagedestroy';
						echo('<br />&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;Destroy temporary gd image&nbsp;&nbsp;<font class="s1 lightgrey">...</font>&nbsp;&nbsp;');
						if(function_exists($gfn))
						{
							$gok=imagedestroy($im);
							if($gok)
								echo($ok.'&nbsp;&nbsp;<span class="green">GD function &quot;'.$gfn.'&quot;: Image could be destroyed.</span><br style="clear:both;" />'.$GLOBALS['nl']);
							else
								echo($er.'&nbsp;&nbsp;<span class="red">GD function &quot;'.$gfn.'&quot;: Image could not be destroyed!</span><br style="clear:both;" />'.$GLOBALS['nl']);
						}
						else
							echo($er.'&nbsp;&nbsp;<span class="red">GD function &quot;'.$gfn.'&quot; does not exist!</span><br style="clear:both;" />'.$GLOBALS['nl']);
					}
					else
						echo($er.'&nbsp;&nbsp;<span class="red">GD function &quot;'.$gfn.'&quot;: Image could not be created!</span><br style="clear:both;" />'.$GLOBALS['nl']);
				}
				else
					echo($er.'&nbsp;&nbsp;<span class="red">GD function &quot;'.$gfn.'&quot; does not exist!</span><br style="clear:both;" />'.$GLOBALS['nl']);
			}
			else
				echo('	&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;'.$er.'&nbsp;&nbsp;<span class="grey">No <b>GD</b> extension found!</span><br style="clear:both;" />'.$GLOBALS['nl']);
			echo('	<br />'.$GLOBALS['nl']);

			// ...Result
			$img_pngo='<img src="'.$GLOBALS['gfxpath'].'gd_test.png" width="'.$iw.'" height="'.$ih.'" align="middle">';
			foreach($sia as $sit => $gds)
			{
				if(file_exists(${'filename_'.$sit}))
				{
					${'col_'.$sit.'c'}='green';
					${'img_'.$sit.'c'}='<img src="'.${'filename_'.$sit}.'?'.$GLOBALS['datetime'].'" width="'.$iw.'" height="'.$ih.'" align="middle">';
				}
				else
				{
					${'col_'.$sit.'c'}='red';
					${'img_'.$sit.'c'}='<b class="red">Image not found!</b>';
				}
			}
			echo('	<div class="s4b" style="padding:2px 8px;background:#ddd;border-bottom:1px solid #000;">Result:</div>'.$GLOBALS['nl']);
			echo('	<table border="0" cellpadding="16" cellspacing="0">'.$GLOBALS['nl']);
			echo('		<tr>'.$GLOBALS['nl']);
			echo('			<td align="center" valign="top"><b class="s5 blue">Comparison Image &rarr;</b></td>'.$GLOBALS['nl']);
			echo('			<td align="center" valign="top"><b class="s5 '.$col_pngc.'">Created PNG Image &darr;</b><br /><i class="s2 grey">(Should look exactly like the comparison image)</i></td>'.$GLOBALS['nl']);
			echo('			<td align="center" valign="top"><b class="s5 '.$col_gifc.'">Created GIF Image &darr;</b><br /><i class="s2 grey">(Should look exactly like the comparison image)</i></td>'.$GLOBALS['nl']);
			echo('			<td align="center" valign="top"><b class="s5 '.$col_jpgc.'">Created JPG Image &darr;</b><br /><i class="s2 grey">(Should look similar to the comparison image)</i></td>'.$GLOBALS['nl']);
			echo('		</tr>'.$GLOBALS['nl']);
			echo('		<tr>'.$GLOBALS['nl']);
			echo('			<td align="center">'.$img_pngo.'</td>'.$GLOBALS['nl']);
			echo('			<td align="center">'.$img_pngc.'</td>'.$GLOBALS['nl']);
			echo('			<td align="center">'.$img_gifc.'</td>'.$GLOBALS['nl']);
			echo('			<td align="center">'.$img_jpgc.'</td>'.$GLOBALS['nl']);
			echo('		</tr>'.$GLOBALS['nl']);
			echo('	</table>'.$GLOBALS['nl']);

			echo('</div>'.$GLOBALS['nl']);
		break;

		case MENU_CHARTDIRECTOR:
			$ea=fxf_checkExtensions($dbt);
//fxDebug($ea,'$ea', 0);

			$cw=200;
			$ch=200;

			$cia=array(1=>'pie', 'bar', 'area');
			foreach($cia as $cnr => $ctx)
			{
				${'filename_'.$ctx}=$GLOBALS['tmppath'].'~cd_test_'.$ctx.'.png';
				if(file_exists(${'filename_'.$ctx}))
					unlink(${'filename_'.$ctx});
			}

			echo('<div style="line-height:26px;">'.$GLOBALS['nl']);

			// ...Checking Extension
			echo('	<div class="s4b" style="padding:2px 8px;background:#ddd;border-bottom:1px solid #000;">Checking Extension:</div>'.$GLOBALS['nl']);
			if($ea['cdext'])
			{
				echo('	&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;'.$ok.'&nbsp;&nbsp;<b>ChartDirector PHP API</b> (Version '.$ea['cdversf'].')<br style="clear:both;" /><br />'.$GLOBALS['nl']);

				// ...Checking Include File
				echo('	<div class="s4b" style="padding:2px 8px;background:#ddd;border-bottom:1px solid #000;">Checking Include File:</div>'.$GLOBALS['nl']);
				$req_fn=fxf_fn_reqFilename('ext_chartdir');
				if(strlen($req_fn))
					require($req_fn);
				if(isset($GLOBALS['_ext_chartdir']) && strlen($GLOBALS['_ext_chartdir']) && strlen($ea['cdok']))
				{
					echo('	&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;'.$ok.'&nbsp;&nbsp;<b>Include file exsists.</b> (Path: '.$ea['cdok'].')<br style="clear:both;" /><br />'.$GLOBALS['nl']);
					// ...Creating ChartDirector Images
					echo('	<div class="s4b" style="padding:2px 8px;background:#ddd;border-bottom:1px solid #000;">Creating ChartDirector Images:</div>'.$GLOBALS['nl']);
					// ...Test #1: Create "Pie"
					echo('	&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;Test #1: Creating &quot;Pie&quot; chart in size '.$cw.'x'.$ch.' as PNG image&nbsp;&nbsp;<font class="s1 lightgrey">...</font>&nbsp;&nbsp;');
					$data=array(42, 18, 8);
					$labels=array('Personal', 'Travel', 'Material');
					$colors=array(0x79c7ae, 0x79aec7, 0xe87986);
					$c=new PieChart($cw, $ch);
					$c->setBackground($c->linearGradientColor(0, 0, 0, 100, 0xe0e0e0, 0xffffff), 0x888888);
					$c->setRoundedFrame(0xfebbbbbb);
					$c->setDropShadow(0xfeaaaaaa);
					$titleObj=$c->addTitle('fx-project: Pie chart test', "arialbd.ttf", 10, 0x000000);
					$titleObj->setBackground(0xdedede, 0x000000, glassEffect());
					$c->setPieSize(floor($cw/2), 10+floor($ch/2), floor($ch/2)-20);
					$c->setLabelPos(-40);
					$c->setData($data, $labels);
					$c->setColors2(DataColor, $colors);
					$c->setSectorStyle(LocalGradientShading, 0x88000000, 1);
					$img=$c->makeChart2(PNG);
					$filename=saveChartDirectorImage($img, $filename_pie);
					if(strlen($filename))
						echo($ok.'&nbsp;&nbsp;<span class="green">Chart created and saved as &quot;'.$filename.'&quot;.</span><br style="clear:both;" />'.$GLOBALS['nl']);
					else if($img)
						echo($er.'&nbsp;&nbsp;<span class="red">Created image could not be saves!</span><br style="clear:both;" />'.$GLOBALS['nl']);
					else
						echo($er.'&nbsp;&nbsp;<span class="red">Chart could not be created!</span><br style="clear:both;" />'.$GLOBALS['nl']);
					// ...Test #2: Create "Bar"
					echo('	&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;Test #3: Creating &quot;Bar&quot; chart in size '.$cw.'x'.$ch.' as PNG image&nbsp;&nbsp;<font class="s1 lightgrey">...</font>&nbsp;&nbsp;');
					$data0=array(100, 125, 245, 147, 67);
					$data1=array(85, 156, 179, 211, 123);
					$data2=array(97, 87, 56, 267, 157);
					$labels=array("Mo", "Tu", "We", "Th", "Fr");
					$c=new XYChart($cw,$ch);
					$c->setBackground($c->linearGradientColor(0, 0, 0, 100, 0xe0e0e0, 0xffffff), 0x888888);
					$c->setRoundedFrame(0xfebbbbbb);
					$c->setDropShadow(0xfeaaaaaa);
					$titleObj=$c->addTitle('fx-project: Bar chart test', "arialbd.ttf", 10, 0x000000);
					$titleObj->setBackground(0xdedede, 0x000000, glassEffect());
					$c->setPlotArea(30, 40, $cw-44, $ch-60);
					$c->xAxis->setLabels($labels);
					$layer=$c->addBarLayer2(Stack, 8);
					$layer->addDataSet($data0, 0x79c7ae, 'Personal');
					$layer->addDataSet($data1, 0x79aec7, 'Travel');
					$layer->addDataSet($data2, 0xe87986, 'Material');
					$layer->setAggregateLabelStyle();
					$layer->setDataLabelStyle();
					$img=$c->makeChart2(PNG);
					$filename=saveChartDirectorImage($img, $filename_bar);
					if(strlen($filename))
						echo($ok.'&nbsp;&nbsp;<span class="green">Chart created and saved as &quot;'.$filename.'&quot;.</span><br style="clear:both;" />'.$GLOBALS['nl']);
					else if($img)
						echo($er.'&nbsp;&nbsp;<span class="red">Created image could not be saves!</span><br style="clear:both;" />'.$GLOBALS['nl']);
					else
						echo($er.'&nbsp;&nbsp;<span class="red">Chart could not be created!</span><br style="clear:both;" />'.$GLOBALS['nl']);
					// ...Test #3: Create "Area"
					echo('	&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;Test #3: Creating &quot;Area&quot; chart in size '.$cw.'x'.$ch.' as PNG image&nbsp;&nbsp;<font class="s1 lightgrey">...</font>&nbsp;&nbsp;');
					$data0=array(87, 89, 85, 66, 53, 39, 24, 21, 37, 56, 37, 22, 21, 33, 13, 17, 4, 23, 16, 25, 9, 10, 5, 7, 6);
					$data1=array(42, 49, 33, 38, 51, 46, 29, 41, 44, 57, 59, 52, 37, 34, 51, 56, 56, 60, 70, 76, 63, 67, 75, 64, 51);
					$data2=array(50, 55, 47, 34, 42, 49, 63, 62, 73, 59, 56, 50, 64, 60, 67, 67, 58, 59, 73, 77, 84, 82, 80, 84, 89);
					$labels=array("0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24");
					$c=new XYChart($cw,$ch);
					$c->setBackground($c->linearGradientColor(0, 0, 0, 100, 0xe0e0e0, 0xffffff), 0x888888);
					$c->setRoundedFrame(0xfebbbbbb);
					$c->setDropShadow(0xfeaaaaaa);
					$titleObj=$c->addTitle('fx-project: Area chart test', "arialbd.ttf", 10, 0x000000);
					$titleObj->setBackground(0xdedede, 0x000000, glassEffect());
					$c->setPlotArea(30, 40, $cw-44, $ch-60);
					$c->xAxis->setLabels($labels);
					$c->xAxis->setLabelStep(4, 1);
					$c->addAreaLayer($data0, 0x80009f6b, 'Personal', 3);
					$c->addAreaLayer($data1, 0x80006b9f, 'Travel', 3);
					$c->addAreaLayer($data2, 0x80e1001a, 'Material', 3);
					$img=$c->makeChart2(PNG);
					$filename=saveChartDirectorImage($img, $filename_area);
					if(strlen($filename))
						echo($ok.'&nbsp;&nbsp;<span class="green">Chart created and saved as &quot;'.$filename.'&quot;.</span><br style="clear:both;" />'.$GLOBALS['nl']);
					else if($img)
						echo($er.'&nbsp;&nbsp;<span class="red">Created image could not be saves!</span><br style="clear:both;" />'.$GLOBALS['nl']);
					else
						echo($er.'&nbsp;&nbsp;<span class="red">Chart could not be created!</span><br style="clear:both;" />'.$GLOBALS['nl']);
				}
				else
					echo($er.'&nbsp;&nbsp;<span class="red">File could not be included!</span><br style="clear:both;" />'.$GLOBALS['nl']);
			}
			else
				echo('	&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;'.$er.'&nbsp;&nbsp;<span class="grey">No <b>ChartDirector PHP API</b> extension found!</span><br style="clear:both;" />'.$GLOBALS['nl']);
			echo('	<br />'.$GLOBALS['nl']);

			// ...Result
			echo('	<div class="s4b" style="padding:2px 8px;background:#ddd;border-bottom:1px solid #000;">Result:</div>'.$GLOBALS['nl']);
			echo('	<table border="0" cellpadding="8" cellspacing="0">'.$GLOBALS['nl']);
			echo('		<tr>'.$GLOBALS['nl']);
			echo('			<td></td>'.$GLOBALS['nl']);
			foreach($cia as $cnr => $ctx)
			{
				if(file_exists(${'filename_'.$ctx}))
				{
					${'col_'.$ctx.'c'}='green';
					${'img_'.$ctx.'c'}='<img src="'.${'filename_'.$ctx}.'?'.$GLOBALS['datetime'].'" width="'.$cw.'" height="'.$ch.'" align="middle">';
				}
				else
				{
					${'col_'.$ctx.'c'}='red';
					${'img_'.$ctx.'c'}='<b class="red">Image not found!</b>';
				}
				echo('			<td align="center" valign="top"><b class="s5 '.${'col_'.$ctx.'c'}.'">Test #'.$cnr.': '.ucfirst($ctx).' &darr;</b></td>'.$GLOBALS['nl']);
			}
			echo('			<td></td>'.$GLOBALS['nl']);
			echo('		</tr>'.$GLOBALS['nl']);
			echo('		<tr>'.$GLOBALS['nl']);
			echo('			<td><b class="s3 blue">Comparison Images &rarr;</b></td>'.$GLOBALS['nl']);
			foreach($cia as $cnr => $ctx)
				echo('			<td align="center"><img src="'.$GLOBALS['gfxpath'].'chart_test_'.$ctx.'.png?'.$GLOBALS['datetime'].'" width="'.$cw.'" height="'.$ch.'" align="middle"></td>'.$GLOBALS['nl']);
			echo('			<td></td>'.$GLOBALS['nl']);
			echo('		</tr>'.$GLOBALS['nl']);
			echo('		<tr>'.$GLOBALS['nl']);
			echo('			<td><b class="s3">Created PNG Images &rarr;</b></td>'.$GLOBALS['nl']);
			foreach($cia as $cnr => $ctx)
				echo('			<td align="center">'.${'img_'.$ctx.'c'}.'</td>'.$GLOBALS['nl']);
			echo('			<td><p align="justify" style="margin-left:32px;width:400px;color:#888;font-style:italic;">If you see a yellow block <b style="color:#000;background:#ff0;">&nbsp;&nbsp;&nbsp;ChartDirector (unregistered)&nbsp;&nbsp;&nbsp;</b> at the bottom of the created PNG images this means that you have either not purchased and/or set a valid ChartDirector license key yourself, or that you have not purchased and/or included any expansion packs from the fx-project manufacturer FeRox Management Consulting GmbH & Co. KG, because this would also set a valid ChartDirector license key indirectly.</p></td>'.$GLOBALS['nl']);
			echo('		</tr>'.$GLOBALS['nl']);
			echo('	</table>'.$GLOBALS['nl']);

			echo('</div>'.$GLOBALS['nl']);
		break;

		case MENU_EMAIL:
			$check=false;
			if(fxIsArray($_POST) && isset($_POST['check_eml']))
				$check=true;

			$emcva=array('ssender','srecipient','ssmtp_host','ismtp_port','ssmtp_helo','psmtp_email','psmtp_account','psmtp_password');
//fxDebug('_POST');
			foreach($emcva as $emcv)
			{
				$emct=substr($emcv,0,1);
				$emcv=substr($emcv,1);

				if($check)
					$GLOBALS['fxpglobals']['dbparam'][$emcv]=$_POST['pv_'.$emcv];
				else if(!$em)
					$GLOBALS['fxpglobals']['dbparam'][$emcv]='';

				if($emct == 'i')
					${$emcv}=(int)$GLOBALS['fxpglobals']['dbparam'][$emcv];
				else
				{
					${$emcv}=$GLOBALS['fxpglobals']['dbparam'][$emcv];
					if($emct == 'p')
					{
						$GLOBALS['fxem'][$emcv]=$GLOBALS['fxpglobals']['dbparam'][$emcv];
						if(substr($GLOBALS['fxem'][$emcv],0,1) == '~')
							$GLOBALS['fxem'][$emcv]=fxf_paramCodec($GLOBALS['fxem'][$emcv]);
						${$emcv}=$GLOBALS['fxem'][$emcv];
					}
				}
//echo('$'.$emcv.' = <b>'.(${$emcv}).'</b><br />');
			}

			$emte='';

			echo('	<div class="s4b" style="line-height:26px;padding:8px;background:#ddd;border-bottom:1px solid #000;">'.$GLOBALS['nl']);
			echo('		<table width="100%">'.$GLOBALS['nl']);
			echo('			<tr>'.$GLOBALS['nl']);
			// ...sender
			if(!strlen($sender) && !$check)
				$sender='fxp@fx-project.org';
			echo('				<td valign="top" nowrap>&nbsp;Sender&nbsp;<br />'.$GLOBALS['nl']);
			$errsa='';
			if(!strlen($sender) && $check)
			{
				$dbtt='Sender cannot be empty!';
				$errsa='border-color:#e1001a;cursor:help;" title="'.$dbtt;
				$emte .= '<b class="s4 red">'.$dbtt.'</b><br />';
			}
			echo('					&nbsp;<input class="fxftxm" type="text" name="pv_sender" tabindex="0" value="'.$sender.'" maxlength="128" style="width:180px;'.$errsa.'">&nbsp;'.$GLOBALS['nl']);
			echo('				</td>'.$GLOBALS['nl']);
			// ...recipient
			$errsa='';
			if(!strlen($recipient) && $check)
			{
				$dbtt='Recipient cannot be empty!';
				$errsa='border-color:#e1001a;cursor:help;" title="'.$dbtt;
				$emte .= '<b class="s4 red">'.$dbtt.'</b><br />';
			}
			echo('				<td valign="top" nowrap>&nbsp;Recipient&nbsp;<br />'.$GLOBALS['nl']);
			echo('					&nbsp;<input class="fxftxm" type="text" name="pv_recipient" tabindex="0" value="'.$recipient.'" maxlength="128" style="width:180px;'.$errsa.'">&nbsp;'.$GLOBALS['nl']);
			echo('				</td>'.$GLOBALS['nl']);
			// ...smtp_host
			echo('				<td valign="top" nowrap>&nbsp;SMTP:&nbsp;Host&nbsp;<br />'.$GLOBALS['nl']);
			echo('					&nbsp;<input class="fxftx" type="text" name="pv_smtp_host" tabindex="0" value="'.$smtp_host.'" maxlength="128" style="width:140px;">&nbsp;'.$GLOBALS['nl']);
			echo('				</td>'.$GLOBALS['nl']);
			// ...smtp_port
			echo('				<td valign="top" nowrap>&nbsp;SMTP:&nbsp;Port&nbsp;<br />'.$GLOBALS['nl']);
			echo('					&nbsp;<input class="fxftx" type="text" name="pv_smtp_port" tabindex="0" value="'.$smtp_port.'" maxlength="6" style="width:80px;">&nbsp;'.$GLOBALS['nl']);
			echo('				</td>'.$GLOBALS['nl']);
			// ...smtp_helo
			echo('				<td valign="top" nowrap>&nbsp;HELO:&nbsp;Identifier&nbsp;<br />'.$GLOBALS['nl']);
			echo('					&nbsp;<input class="fxftx" type="text" name="pv_smtp_helo" tabindex="0" value="'.$smtp_helo.'" maxlength="128" style="width:300px;">&nbsp;'.$GLOBALS['nl']);
			echo('				</td>'.$GLOBALS['nl']);
			// ...smtp_email
			echo('				<td valign="top" nowrap>&nbsp;AUTH:&nbsp;EMail&nbsp;Address&nbsp;<br />'.$GLOBALS['nl']);
			echo('					&nbsp;<input class="fxftx" type="text" name="pv_smtp_email" tabindex="0" value="'.$smtp_email.'" maxlength="128" style="width:180px;">&nbsp;'.$GLOBALS['nl']);
			echo('				</td>'.$GLOBALS['nl']);
			// ...smtp_account
			echo('				<td valign="top" nowrap>&nbsp;AUTH:&nbsp;Account&nbsp;<br />'.$GLOBALS['nl']);
			echo('					&nbsp;<input class="fxftx" type="text" name="pv_smtp_account" tabindex="0" value="'.$smtp_account.'" maxlength="128" style="width:180px;">&nbsp;'.$GLOBALS['nl']);
			echo('				</td>'.$GLOBALS['nl']);
			// ...smtp_password
			echo('				<td valign="top" nowrap>&nbsp;AUTH:&nbsp;Password&nbsp;<br />'.$GLOBALS['nl']);
			echo('					&nbsp;<input class="fxftx" type="password" name="pv_smtp_password" tabindex="0" value="'.$smtp_password.'" maxlength="32" style="width:140px;">&nbsp;'.$GLOBALS['nl']);
			echo('				</td>'.$GLOBALS['nl']);
			// ...Button
			echo('				<td width="100%" align="right" nowrap>'.$GLOBALS['nl']);
			echo('					<div class="btn" style="display:inline;" '.fxf_jsFunction('load', $menu_selected, 'check_eml').'>&nbsp;Send&nbsp;Emails&nbsp;</div>'.$GLOBALS['nl']);
			echo('				</td>'.$GLOBALS['nl']);
			echo('			</tr>'.$GLOBALS['nl']);
			echo('		</table>'.$GLOBALS['nl']);
			echo('	</div>'.$GLOBALS['nl']);

			// Error?
			if(strlen($emte))
				echo('<br />'.$emte);
			// Checks?
			else if($check)
			{
				$ea=fxf_checkExtensions($dbt);
//fxDebug($ea,'$ea', 0);
				$sock=false;
				if(fxIsArray($ea['extstat']) && $ea['extstat']['sockets'])
					$sock=true;

				$GLOBALS['force_recipient']=true;

				// Get external: mailer (PHPMailer)
				if(!isset($GLOBALS['_ext_mailer']) || !strlen($GLOBALS['_ext_mailer']))
				{
					$GLOBALS['__includer']['ext_mailer']=true;
					require('includer.inc');
				}

				echo('	<br style="clear:both;" /><br />'.$GLOBALS['nl']);
				echo('	<div style="line-height:26px;">'.$GLOBALS['nl']);

				// SMTP
				echo('	<div class="s4b" style="padding:2px 8px;background:#ddd;border-bottom:1px solid #000;">SMTP Email Test:</div>'.$GLOBALS['nl']);
				echo('	&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;Sending email to &quot;'.$recipient.'&quot;&nbsp;<font class="s1 lightgrey">...</font>&nbsp;&nbsp;');
				if($sock)
				{
					if(isset($GLOBALS['fxpglobals']['dbparam']['smtp_host']) && strlen($GLOBALS['fxpglobals']['dbparam']['smtp_host']))
					{
						if(isset($GLOBALS['_ext_mailer']) && strlen($GLOBALS['_ext_mailer']))
						{
							$error=send_mail($recipient, 'fx-project: SMTP Mail Test ['.$GLOBALS['datetime'].']', 'Send by fx-project via SMTP connection to test email functionality.', $sender, 0, false, false, true);
//fxDebug($error,'$error: send_mail', 0);
							if($error)
							{
								$emsg='';
								if(fxIsArray($error))
								{
									foreach($error as $em)
									{
										if(strlen($emsg))
											$emsg .= ', ';
										$emsg .= '['.$em.']';
									}
								}
								else if($error === true)
									$emsg='[Unspecified error!]';
								else
									$emsg='['.$error.']';
								echo($er.'&nbsp;&nbsp;<span class="red">'.$emsg.'</span><br style="clear:both;" />'.$GLOBALS['nl']);
							}
							else
								echo($ok.'&nbsp;&nbsp;<span class="green">No errors returned - Email should have been send successfully.</span><br style="clear:both;" />'.$GLOBALS['nl']);
						}
						else
							echo($ko.'&nbsp;&nbsp;<span class="green">No external mailer (like e.g. PHPMailer) integrated!</span><br style="clear:both;" />'.$GLOBALS['nl']);
					}
					else
						echo($sk.'&nbsp;&nbsp;<span class="darkgrey">No <b>SMTP Host</b> specified!</span><br style="clear:both;" />'.$GLOBALS['nl']);
				}
				else
					echo($sk.'&nbsp;&nbsp;<span class="darkgrey">PHP extension &quot;<b>sockets</b>&quot; not integrated into PHP!</span><br style="clear:both;" />'.$GLOBALS['nl']);

				// PHP
				$GLOBALS['fxpglobals']['dbparam']['smtp_host']='';
				echo('	<br /><div class="s4b" style="padding:2px 8px;background:#ddd;border-bottom:1px solid #000;">PHP Email Test:</div>'.$GLOBALS['nl']);
				echo('	&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;Sending email to &quot;'.$recipient.'&quot;&nbsp;<font class="s1 lightgrey">...</font>&nbsp;&nbsp;');
				$error=send_mail($recipient, 'fx-project: PHP Mail Test ['.$GLOBALS['datetime'].']', 'Send by fx-project via PHP standard mail function to test email functionality.', $sender, 0, false, false, true);
				if($error)
				{
//fxDebug($error,'$error: send_mail', 0);
					$emsg='';
					if(fxIsArray($error))
					{
						foreach($error as $em)
						{
							if(strlen($emsg))
								$emsg .= ', ';
							$emsg .= '['.$em.']';
						}
					}
					else if($error === true)
						$emsg='[Unspecified error!]';
					else
						$emsg='['.$error.']';
					echo($er.'&nbsp;&nbsp;<span class="red">'.$emsg.'</span><br style="clear:both;" />'.$GLOBALS['nl']);
				}
				else
					echo($ok.'&nbsp;&nbsp;<span class="green">The standard mail function of PHP returned success. This means the email was successfully forwarded to the assigned email program. It does not necessarily mean the email was correctly sent. Please check your email inbox to verify.</span><br style="clear:both;" />'.$GLOBALS['nl']);

				echo('	</div>'.$GLOBALS['nl']);
			}
		break;

		case MENU_DB_CONNECTION:
			$check=false;
			if(fxIsArray($_POST) && isset($_POST['check_dbc']))
				$check=true;

			$dbcva=array('idbtype','sdbserver','sdbname','pusername','ppassword');
//fxDebug('_POST');
			foreach($dbcva as $dbcv)
			{
				$dbct=substr($dbcv,0,1);
				$dbcv=substr($dbcv,1);

				if($check)
					$GLOBALS['fxpglobals']['dbparam'][$dbcv]=$_POST['pv_'.$dbcv];
				else if(!$em)
					$GLOBALS['fxpglobals']['dbparam'][$dbcv]='';

				if($dbct == 'i')
					${$dbcv}=(int)$GLOBALS['fxpglobals']['dbparam'][$dbcv];
				else
				{
					${$dbcv}=$GLOBALS['fxpglobals']['dbparam'][$dbcv];
					if($dbct == 'p')
					{
						$GLOBALS['fxdb'][$dbcv]=$GLOBALS['fxpglobals']['dbparam'][$dbcv];
						if(substr($GLOBALS['fxdb'][$dbcv],0,1) == '~')
							$GLOBALS['fxdb'][$dbcv]=fxf_paramCodec($GLOBALS['fxdb'][$dbcv]);
						${$dbcv}=$GLOBALS['fxdb'][$dbcv];
					}
				}
//echo('$'.$dbcv.' = <b>'.(${$dbcv}).'</b><br />');
			}

			if(!strlen($GLOBALS['fxpglobals']['dbparam']['dbserver']))
				$GLOBALS['fxpglobals']['dbparam']['dbserver']='localhost';

			$GLOBALS['dbt']=$GLOBALS['_dbtypes'][$GLOBALS['fxpglobals']['dbparam']['dbtype']]['type'];
			$GLOBALS['db_concatsign']='+';
			if($GLOBALS['dbt'] == 'pgsql')
				$GLOBALS['db_concatsign']='||';

			$dbte='';

			echo('	<div class="s4b" style="line-height:26px;padding:8px;background:#ddd;border-bottom:1px solid #000;">'.$GLOBALS['nl']);
			echo('		<table width="100%">'.$GLOBALS['nl']);
			echo('			<tr>'.$GLOBALS['nl']);
			// ...dbtype
			echo('				<td valign="top" nowrap>&nbsp;Type&nbsp;<br />'.$GLOBALS['nl']);
			echo('					&nbsp;<select name="pv_dbtype" style="width:140px;height:22px;color:#006b9f;font-weight:bold;">'.$GLOBALS['nl']);
			foreach($GLOBALS['_dbtypes'] as $tid => $tta)
			{
				$sel='';
				if($dbtype == $tid)
					$sel=' selected';
				echo('						<option value="'.$tid.'"'.$sel.' style="color:#000000;font-weight:normal;">'.$tta['text'].'</option>'.$GLOBALS['nl']);
			}
			echo('					</select>&nbsp;'.$GLOBALS['nl']);
			echo('				</td>'.$GLOBALS['nl']);
			// ...dbserver
			echo('				<td valign="top" nowrap>&nbsp;Connection&nbsp;String&nbsp;(Server)&nbsp;<br />'.$GLOBALS['nl']);
			echo('					&nbsp;<input class="fxftx" type="text" name="pv_dbserver" tabindex="0" value="'.$dbserver.'" maxlength="128" style="width:256px;">&nbsp;');
			echo('				</td>'.$GLOBALS['nl']);
			// ...dbname
			$errsa='';
			if(!strlen($dbname) && $check)
			{
				$dbtt='Database name cannot be empty!';
				$errsa='border-color:#e1001a;cursor:help;" title="'.$dbtt;
				$dbte .= '<b class="s4 red">'.$dbtt.'</b><br />';
			}
			echo('				<td valign="top" nowrap>&nbsp;Database&nbsp;Name&nbsp;<br />'.$GLOBALS['nl']);
			echo('					&nbsp;<input class="fxftxm" type="text" name="pv_dbname" tabindex="0" value="'.$dbname.'" maxlength="64" style="width:164px;'.$errsa.'">&nbsp;');
			echo('				</td>'.$GLOBALS['nl']);
			// ...username
			$errsa='';
			if(!strlen($username) && $check)
			{
				$dbtt='Username cannot be empty!';
				$errsa='border-color:#e1001a;cursor:help;" title="'.$dbtt;
				$dbte .= '<b class="s4 red">'.$dbtt.'</b><br />';
			}
			echo('				<td valign="top" nowrap>&nbsp;Username&nbsp;<br />'.$GLOBALS['nl']);
			echo('					&nbsp;<input class="fxftxm" type="text" name="pv_username" tabindex="0" value="'.$username.'" maxlength="32" style="width:128px;'.$errsa.'">&nbsp;');
			echo('				</td>'.$GLOBALS['nl']);
			// ...password
			echo('				<td valign="top" nowrap>&nbsp;Password&nbsp;<br />'.$GLOBALS['nl']);
			echo('					&nbsp;<input class="fxftx" type="password" name="pv_password" tabindex="0" value="'.$password.'" maxlength="32" style="width:128px;">&nbsp;');
			echo('				</td>'.$GLOBALS['nl']);
			// ...Button
			echo('				<td width="100%" align="right" nowrap>'.$GLOBALS['nl']);
			echo('					<div class="btn" style="display:inline;" '.fxf_jsFunction('load', $menu_selected, 'check_dbc').'>&nbsp;Check&nbsp;database&nbsp;connection&nbsp;</div>'.$GLOBALS['nl']);
			echo('				</td>'.$GLOBALS['nl']);
			echo('			</tr>'.$GLOBALS['nl']);
			echo('		</table>'.$GLOBALS['nl']);
			echo('	</div>'.$GLOBALS['nl']);

			// Error?
			if(strlen($dbte))
				echo('<br />'.$dbte);
			// Checks?
			else if($check)
			{
				echo('	<br style="clear:both;" /><br />'.$GLOBALS['nl']);
				echo('	<div style="line-height:26px;">'.$GLOBALS['nl']);

				// ...Check Connection
				echo('	<div class="s4b" style="padding:2px 8px;background:#ddd;border-bottom:1px solid #000;">Check Connection:</div>'.$GLOBALS['nl']);
				echo('	&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;Testing database connection to the <b>'.$GLOBALS['dbt'].'</b> database &quot;<b>'.$GLOBALS['fxpglobals']['dbparam']['dbname'].'</b>&quot; with the connection string &quot;<b>'.$GLOBALS['fxpglobals']['dbparam']['dbserver'].'</b>&quot;, the username &quot;<b>'.$GLOBALS['fxdb']['username'].'</b>&quot; and the entered password&nbsp;<font class="s1 lightgrey">...</font>&nbsp;&nbsp;');
				if($GLOBALS['dbid'])
					echo($ok.'&nbsp;&nbsp;<span class="green">Connection successfull</span><br style="clear:both;" />'.$GLOBALS['nl']);
				else
				{
					$es=db_err();
					if(!strlen($es))
						$es='Could not connect to database!';
					echo($er.'&nbsp;&nbsp;<span class="red">'.$es.'</span><br style="clear:both;" />'.$GLOBALS['nl']);
				}

				// ...Specifications
				if($GLOBALS['dbid'])
				{
					echo('	<br /><div class="s4b" style="padding:2px 8px;background:#ddd;border-bottom:1px solid #000;">Specifications:</div>'.$GLOBALS['nl']);
					db_version();
					db_param();
					echo('	&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;Reading database specifications #1&nbsp;<font class="s1 lightgrey">...</font>&nbsp;&nbsp;');
					$spc='<b>Server:</b> '.$GLOBALS['fxpglobals']['dbparam']['dbvers']['name'].', <b>Version:</b> '.$GLOBALS['fxpglobals']['dbparam']['dbvers']['version'].' = <b>Vers:</b> '.$GLOBALS['fxpglobals']['dbparam']['dbvers']['vers'];
					echo($in.'&nbsp;&nbsp;<span class="blue">'.$spc.'</span><br style="clear:both;" />'.$GLOBALS['nl']);
					echo('	&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;Reading database specifications #2&nbsp;<font class="s1 lightgrey">...</font>&nbsp;&nbsp;');
					$spc='<b>Cast:</b> '.$GLOBALS['fxpglobals']['dbparam']['dbcast'].', <b>Cascade:</b> ';
					if(strlen($GLOBALS['fxpglobals']['dbparam']['dbcascade']))
						$spc .= 'ON ['.$GLOBALS['fxpglobals']['dbparam']['dbcascade'].']';
					else
						$spc .= '<i>OFF</i>';
					echo($in.'&nbsp;&nbsp;<span class="blue">'.$spc.'</span><br style="clear:both;" />'.$GLOBALS['nl']);
				}

				// ...Table
				$lte=false;
				if($GLOBALS['dbid'])
				{
					echo('	<br /><div class="s4b" style="padding:2px 8px;background:#ddd;border-bottom:1px solid #000;">Table:</div>'.$GLOBALS['nl']);
					echo('	&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;Check if lookup table &quot;<b>wertetab</b>&quot; exists&nbsp;<font class="s1 lightgrey">...</font>&nbsp;&nbsp;');
					$lte=db_check('wertetab','t');
					if($lte)
						echo($ok.'&nbsp;&nbsp;<span class="green">Table exists.</span><br style="clear:both;" />'.$GLOBALS['nl']);
					else
						echo($er.'&nbsp;&nbsp;<span class="red">Table does not exist!</span><br style="clear:both;" />'.$GLOBALS['nl']);
				}

				// ...Currency
				if($lte)
				{
					echo('	<br /><div class="s4b" style="padding:2px 8px;background:#ddd;border-bottom:1px solid #000;">Currency:</div>'.$GLOBALS['nl']);
					echo('	&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;Reading euro currency symbol from lookup values - should look like this: <b>&euro;</b>&nbsp;<font class="s1 lightgrey">...</font>&nbsp;&nbsp;');
					$sql="SELECT tabwert FROM wertetab WHERE wertetabid=490 AND id_sprache=1 AND mandanten_id=0";
					$eur=db_value($sql);
					if($eur === '€')
						echo($ok.'&nbsp;&nbsp;<span class="green">Currency symbol is &quot;'.$eur.'&quot;.</span><br style="clear:both;" />'.$GLOBALS['nl']);
					else
						echo($ok.'&nbsp;&nbsp;<span class="red">Currency symbol is &quot;'.$eur.'&quot;.</span><br style="clear:both;" />'.$GLOBALS['nl']);
				}

				// ...Tests
				if($lte)
				{
					echo('	<br /><div class="s4b" style="padding:2px 8px;background:#ddd;border-bottom:1px solid #000;">Tests:</div>'.$GLOBALS['nl']);

					$wst=0;
					$str='123 ABC abc äöü ÄÖÜ ß € " \' ~ $ & ? + - /\\ \\\\//';
					echo('	&nbsp;&nbsp;<span class="grey">Test string:</span>&nbsp;&quot;<b class="darkgrey">'.fxHtmlEncode($str).'</b>&quot;<br />'.$GLOBALS['nl']);

					// ...Insert
					echo('	&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;Inserting test string into lookup table&nbsp;<font class="s1 lightgrey">...</font>&nbsp;&nbsp;');
					$sql="SELECT tabwert FROM wertetab WHERE wertetabid=99999 AND id_sprache=1 AND mandanten_id=0";
					$chk=db_value($sql);
					if(!strlen($chk))
					{
						$sql  = "INSERT INTO wertetab (mandanten_id,wertetabid,id_sprache,id_feld,position,tabwert,satzvers,archiv,transid,aenderungs_id,zeitstempel)";
						$sql .= " VALUES (0,99999,1,0,0,'".convert_string($str,'todb')."',0,0,0,0,'".$GLOBALS['datetime']."')";
//debug2($sql,'$sql');
						$success=db_query($sql);
						if($success)
							$wst=1;
					}
					else
						$wst=2;
					if(!$wst)
					{
						$es=db_err();
						if(!strlen($es))
							$es='Could not insert test string!';
						echo($er.'&nbsp;&nbsp;<span class="red">'.$es.'</span><br style="clear:both;" />'.$GLOBALS['nl']);
					}
					else if($wst > 1)
						echo($sk.'&nbsp;&nbsp;<span class="grey">Test string exsists already.</span><br style="clear:both;" />'.$GLOBALS['nl']);
					else
						echo($ok.'&nbsp;&nbsp;<span class="green">Test string inserted successfully.</span><br style="clear:both;" />'.$GLOBALS['nl']);

					// ...Select
					if($wst)
					{
						echo('	&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;Reading and comparing test string from lookup table&nbsp;<font class="s1 lightgrey">...</font>&nbsp;&nbsp;');
						$sql="SELECT tabwert FROM wertetab WHERE wertetabid=99999 AND id_sprache=1 AND mandanten_id=0";
						$chk=db_value($sql);
						if($chk === $str)
							echo($ok.'&nbsp;&nbsp;<span class="green">Test strings <b>MATCH</b>.</span><br style="clear:both;" />'.$GLOBALS['nl']);
						else
							echo($er.'&nbsp;&nbsp;<span class="red">Test string <b>DON\'T MATCH</b> - Value from table is &quot;'.fxHtmlEncode($chk).'&quot;!</span><br style="clear:both;" />'.$GLOBALS['nl']);
					}

					// ...Delete
					if($wst)
					{
						echo('	&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;Deleting test string from lookup table&nbsp;<font class="s1 lightgrey">...</font>&nbsp;&nbsp;');
						$sql="DELETE FROM wertetab WHERE wertetabid=99999 AND mandanten_id=0";
						$success=db_query($sql);
						if($success)
							$wst=0;
						if($wst)
						{
							$es=db_err();
							if(!strlen($es))
								$es='Could not delete test string!';
							echo($er.'&nbsp;&nbsp;<span class="red">'.$es.'</span><br style="clear:both;" />'.$GLOBALS['nl']);
						}
						else
							echo($ok.'&nbsp;&nbsp;<span class="green">Test string deleted successfully.</span><br style="clear:both;" />'.$GLOBALS['nl']);
					}
				}

				if($GLOBALS['dbid'])
					db_schliessen();

				echo('	</div>'.$GLOBALS['nl']);
			}
		break;

		case MENU_DB_CHECKER:
			echo('	<div style="line-height:26px;">'.$GLOBALS['nl']);
			// ...Check Connection
			echo('	<div class="s4b" style="padding:2px 8px;background:#ddd;border-bottom:1px solid #000;">Check Connection:</div>'.$GLOBALS['nl']);
			echo('	&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;Testing database connection to the database with the parameters from the &quot;fx_project.ini&quot; file&nbsp;<font class="s1 lightgrey">...</font>&nbsp;&nbsp;');
			if($GLOBALS['dbid'])
			{
				echo($ok.'&nbsp;&nbsp;<span class="green">Connection successfull</span><br style="clear:both;" />'.$GLOBALS['nl']);

				// Check Definition Text File
				$filedir=$GLOBALS['datpath'];
				$prgvers=$GLOBALS['fxpglobals']['dbparam']['versionscode_prg'];
				$dbspath=$filedir.$prgvers.'_vrs.txt';
				$dbsot=array('tab'=>'Table structure', 'ind'=>'Table index', 'dat'=>'Table data');
				$cerr=0;
				echo('	<br /><div class="s4b" style="padding:2px 8px;background:#ddd;border-bottom:1px solid #000;">Check Definition Text File in Directory &quot;'.$filedir.'&quot;:</div>'.$GLOBALS['nl']);
				echo('	&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;Check file &quot;<b>'.$prgvers.'_vrs.txt</b>&quot;&nbsp;<font class="s1 lightgrey">...</font>&nbsp;&nbsp;');
				if(!file_exists($dbspath) || !filesize($dbspath))
				{
					$cerr++;
					echo($ko.'&nbsp;&nbsp;<span class="red">File not found or empty!</span><br style="clear:both;" />'.$GLOBALS['nl']);
				}
				else
				{
					// Get definition file lines as array
					$fa=fxLoad($dbspath, 1);
					if(fxIsArray($fa))
						echo($ok.'&nbsp;&nbsp;<span class="green">File exists containing '.sizeof($fa).' lines.</span><br style="clear:both;" />'.$GLOBALS['nl']);
					else
					{
						$cerr++;
						echo($ko.'&nbsp;&nbsp;<span class="red">File exists, but is empty!</span><br style="clear:both;" />'.$GLOBALS['nl']);
					}
				}

				// Check Tables
				if(!$cerr)
				{
					dbStructure();

					echo('	<br /><div class="s4b" style="padding:2px 8px;background:#ddd;border-bottom:1px solid #000;">Check Tables:</div>'.$GLOBALS['nl']);
					foreach($dbsot as $dbs => $dbt)
					{
						echo('	&nbsp;&nbsp;<span class="grey">&rarr;</span>&nbsp;Check</font> [<b>'.$dbt.'</b>]&nbsp;<font class="s1 lightgrey">...</font>&nbsp;&nbsp;');
						if($dbs == 'tab')
							$pa=dbGetTablePKDiff($prgvers,$fa,0);
						else if($dbs == 'ind')
							$pa=dbGetIndexDiff($prgvers,0,false,$fa);
						else if($dbs == 'dat')
							$pa=dbGetDataDiff($prgvers,0,$fa);
						else
							$pa=array();
//fxDebug($pa,'$pa: $dbs='.$dbs, 0);

						if(fxIsArray($pa) && ($pa['rellines'] > 0))
						{
							$sec=0;
							if($pa['status'] >= 0)
							{
								foreach($pa['sqlarray'] as $sql)
								{
									if(substr($sql,0,1) != '*')
										$sec++;
								}
							}
							if(($pa['status'] < 0) || $sec)
							{
								$pcp=strpos($pa['screencontent'], '<!--#CHECK#-->');
								$psc=trim(substr($pa['screencontent'], $pcp+14));
								if($pa['status'] < 0)
									echo($ko.'&nbsp;&nbsp;<span class="darkgrey">'.$pa['rellines'].'/'.$pa['filelines'].' definition lines read and checked.</span> <b class="red">&rarr; '.$pa['message'].'</b><br style="clear:both;" /><div style="margin-left:26px;padding:6px;border:1px solid #ddd;border-radius:8px;">'.$psc.'</div>'.$GLOBALS['nl']);
								else
									echo($in.'&nbsp;&nbsp;<span class="darkgrey">'.$pa['rellines'].'/'.$pa['filelines'].' definition lines read and checked.</span> <b>&rarr; '.$pa['message'].'</b><br style="clear:both;" /><div style="margin-left:26px;padding:6px;border:1px solid #ddd;border-radius:8px;">'.$psc.'</div>'.$GLOBALS['nl']);
							}
							else
								echo($ok.'&nbsp;&nbsp;<span class="green">'.$pa['rellines'].'/'.$pa['filelines'].' definition lines read and checked.</span><br style="clear:both;" />'.$GLOBALS['nl']);
						}
						else
							echo($sk.'&nbsp;&nbsp;<span class="red">No relevant definition entries!</span><br style="clear:both;" />'.$GLOBALS['nl']);
					}
				}
			}
			else
			{
				$es=db_err();
				if(!strlen($es))
					$es='Could not connect to database!';
				echo($er.'&nbsp;&nbsp;<span class="red">'.$es.'</span><br style="clear:both;" />'.$GLOBALS['nl']);
			}

			if($GLOBALS['dbid'])
				db_schliessen();

			echo('	</div>'.$GLOBALS['nl']);
		break;

		default:
			$whtml .= '<br /><span style="font-size:16pt;color:#444;">Hello, please select an entry from the menu bar above.</span>'.$GLOBALS['nl'];
		break;
	}

	// Get screen content
	$output=ob_get_contents();
	ob_end_clean();

	// Menu
	$display='	<div style="position:fixed;left:0;top:48px;right:0;height:30px;padding-top:10px;background:rgba(0,0,0, 0.9);border-bottom:1px solid #000;box-shadow:0 2px 4px rgba(0,0,0, 0.66);z-index:900;">'.$GLOBALS['nl'];
	$mec=0;
	foreach($menu_array as $menu_id => $menu_text)
	{
		$mec++;

		$class_add='';
		if($menu_selected == $menu_id)
			$class_add='a';

		$mok=true;
		if(($menu_id == MENU_DB_CONNECTION) || ($menu_id == MENU_DB_CHECKER))
		{
			$mok=false;
			if(!$dberror && (fxIsArray($GLOBALS['fxpglobals']['dbparam']) && ((isset($GLOBALS['fxpglobals']['dbparam']['dbserver']) && strlen($GLOBALS['fxpglobals']['dbparam']['dbserver'])) || (isset($GLOBALS['fxpglobals']['dbparam']['dbname']) && strlen($GLOBALS['fxpglobals']['dbparam']['dbname'])))) && (($menu_id == MENU_DB_CONNECTION) || $GLOBALS['odbid']))
				$mok=true;
		}

		if($mok)
			$display .= '		<input name="menu_id_'.$menu_id.'" class="mn'.$class_add.'s" type="submit" value="&nbsp;'.$menu_text.'&nbsp;" style="cursor:pointer;" '.fxf_jsFunction('load', $menu_id).'>'.$GLOBALS['nl'];
		else
			$display .= '		<span class="mni">&nbsp;'.$menu_text.'&nbsp;</span>'.$GLOBALS['nl'];

		if($mec < sizeof($menu_array))
			$display .= '		<span style="font-size:12pt;color:rgba(255,255,255, 0.25);">|</span>'.$GLOBALS['nl'];
	}
	$display .= '	</div>'.$GLOBALS['nl'];

	$mwa_style='background:transparent;';
	$mwa_top=97;
	if($menu_selected > 0)
	{
		$mwa_style='line-height:20px;background:#fefefe;border:1px solid #444;box-shadow:2px 2px 4px inset rgba(0,0,0, 0.66);';
		$mwa_top=133;

		if(!isset($htitle) || !strlen($htitle))
			$htitle=$menu_array[$menu_selected];

		$display .= '	<div class="fxheaderc" style="position:fixed;left:8px;top:97px;right:8px;height:20px;padding:8px;z-index:800;">'.$GLOBALS['nl'];
		$display .= '		<span class="fxheader" style="padding-left:0;">&rarr; '.$htitle.' &hellip;</span>'.$GLOBALS['nl'];
		$display .= '	</div>'.$GLOBALS['nl'];
	}

	// Output
	$display .= '	<div style="position:fixed;left:8px;top:'.$mwa_top.'px;right:8px;bottom:12px;padding:8px;color:#000;'.$mwa_style.'overflow:auto;z-index:500;">'.$GLOBALS['nl'];
	$display .= '		<form id="workform" name="workform" action="'.basename(__FILE__).'" method="post">'.$GLOBALS['nl'];
	$display .= $output.$whtml;
	$display .= '		</form>'.$GLOBALS['nl'];
	$display .= '	</div>'.$GLOBALS['nl'];
	$display .= '	<div id="runtime" style="position:fixed;right:4px;bottom:2px;color:rgb(0,0,0, 0.9);font-size:5pt;"><!--#RUNTIME#--></div>'.$GLOBALS['nl'];
}

// Runtime END
list($usec,$sec)=explode(' ',microtime());
$rte=$sec.substr($usec,1);
$rtd=$rte-$rts;
$runtime='Runtime: '.$rts.'-'.$rte.' &rarr; <b style="font-size:5pt;">'.$rtd.'</b> sec.';

// Display content (with runtime)
if(strlen($html_head))
	$display='<!DOCTYPE HTML>'.$GLOBALS['nl'].'<html>'.$GLOBALS['nl'].$html_head.$html_body.'</html>';

echo(str_replace('<!--#RUNTIME#-->',$runtime,$display));

// Delete status
fxProgressStatus();
?>