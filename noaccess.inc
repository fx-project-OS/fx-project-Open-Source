<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : noaccess.inc                                                 //
// Version     : 21.2                                                         //
// Begin       : 2020-07-28                                                   //
// Last Change : 2020-07-28                                                   //
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
 * Default html display text if a user is not allowed to access a file!
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

// Send these additional headers
$__headers=array(
	'Last-Modified: '.gmdate('D, d M Y H:i:s').' GMT',	// current date, because it is created differently ever time dynamically
	'Cache-Control: no-cache, must-revalidate',  		// HTTP/1.1
	'Pragma: no-cache'                         			// HTTP/1.0
);

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

// HTML header
$_ecolor='ffffff';
if(!isset($_title) || !strlen($_title))
{
	$_ecolor='e1001a';
	$_title='Attention!';
	if($GLOBALS['fxpglobals']['lang'] == 1)	// Deutsch
		$_title='Achtung!';
}
$html_head  = '<head>'.$GLOBALS['nl'];
$html_head .= '	<title>fx-project '.$_title.'</title>'.$GLOBALS['nl'];
$html_head .= '	<meta http-equiv="content-type" content="text/html; charset='.FXP_CHARSET.'">'.$GLOBALS['nl'];
$html_head .= '	<link rel="shortcut icon" href="'.fxGetFavicon().'">'.$GLOBALS['nl'];
$html_head .= '	<link rel="stylesheet" type="text/css" href="'.$GLOBALS['csspath'].'00default.css">'.$GLOBALS['nl'];
$html_head .= '</head>'.$GLOBALS['nl'];

// HTML body with title bar
$html_body  = '<body class="fxbody">'.$GLOBALS['nl'];
// ...Title bar
$html_body .= '	<div class="fxtoolc nousersel" style="position:fixed;left:0;top:0;right:0;height:47px;z-index:150;">'.$GLOBALS['nl'];
$html_body .= '		<img id="fxswlogobg" src="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAFkAAAAvCAYAAACIcGZzAAABA0lEQVR4nO3QsQ1CQRAD0emJNqiDNmidgPATILizfdauNPn6ATyn5T2AO++7EfBQWxfgQRYAD7IAeJAFwIMsAB5kAfAgC4AHWQA8yALgQRYAD7IAeJAFwAQMSO9vYAJGJLcEmIAhqS0DJmBMYkuBCRiU1nJgAkYltQWYgGEpbQMmYFxCW4EJGOhuOzABI+uBCRhaD4xhXEJSYASD0pIDs2FEchZgFjx+SjZgfnj2xKzAfPHg6dmB+fBUUxHA4IeoBwY/Rj0w+EHqgcGPUg8Mfph6YPDj1AODH6geGPxI9cDgh6oHBj9WPTD4weqBwY9WDwx+uHpg8OPVA4MfsB4Y/Ij1wC+9Mc/1UdaqWQAAAABJRU5ErkJggg==">'.$GLOBALS['nl'];
$html_body .= '		'.($GLOBALS['_elinks']?'<a href="https://www.fx-project.org" target="_blank">':'').'<img id="fxswlogo" src="'.fxGetLogo('small').'" tooltip="fx-project '.$v_type.' V '.FXP_VERSION.'">'.($GLOBALS['_elinks']?'</a>':'').$GLOBALS['nl'];
$html_body .= '		<div style="position:absolute;left:88px;top:4px;text-shadow:2px 2px 2px rgba(0,0,0, 0.5);"><span style="color:#fff;font-size:22pt;">f</span><span style="color:#e1001a;font-size:20pt;">x</span><span style="color:#001a26;font-size:18pt;">-project</span></div>'.$GLOBALS['nl'];
$html_body .= '		<div style="position:absolute;right:8px;top:9px;color:#'.$_ecolor.';font-size:18pt;font-weight:bolder;text-shadow:2px 2px 2px rgba(0,0,0, 0.5);">'.$_title.'</div>'.$GLOBALS['nl'];
$html_body .= '	</div>'.$GLOBALS['nl'];
// ...Display area
$html_body .= '	<div style="position:fixed;left:0;top:48px;right:0;bottom:0;">'.$GLOBALS['nl'];
$html_body .= '		<table width=100% height=100% border=0 cellpadding=0 cellspacing=0>'.$GLOBALS['nl'];
$html_body .= '			<tr>'.$GLOBALS['nl'];
$html_body .= '				<td width=100% height=100" align=center valign=middle>'.$GLOBALS['nl'];
if(isset($_message) && strlen($_message))
	$html_body .= $_message;
else
{
	$_message='You are NOT allowed to access this subdirectory!';
	if($GLOBALS['fxpglobals']['lang'] == 1)	// Deutsch
		$_message='Sie sind NICHT berechtigt auf dieses Unterverzeichnis zuzugreifen!';
	$html_body .= '					<span style="font-size:22pt;font-weight:bolder;color:#fff;text-shadow:2px 2px 2px rgba(0,0,0, 0.66);">'.$_message.'</span><br /><br />'.$GLOBALS['nl'];
}
$html_body .= '				</td>'.$GLOBALS['nl'];
$html_body .= '			</tr>'.$GLOBALS['nl'];
$html_body .= '		</table>'.$GLOBALS['nl'];
$html_body .= '	</div>'.$GLOBALS['nl'];
// ...Copyright
$html_body .= '	<div style="position:fixed;right:12px;bottom:8px;color:#112;font-size:12pt;font-weight:bolder;text-shadow:1px 1px 1px rgba(0,0,0, 0.5);">'.$copyright_string.'</div>'.$GLOBALS['nl'];
$html_body .= '</body>'.$GLOBALS['nl'];

echo('<!DOCTYPE HTML>'.$GLOBALS['nl'].'<html>'.$GLOBALS['nl'].$html_head.$html_body.'</html>');
?>