<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : filemanager.inc                                              //
// Version     : 21.2                                                         //
// Begin       : 2020-11-20                                                   //
// Last Change : 2020-12-17                                                   //
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
 * Download or display a file
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

// Title
$_title=meldung(1578, false);	// File-Manager

// Get submitted GET variables
$_error='';
$_ga=array('pa'=>0, 'type'=>'', 'filename'=>'', 'manual'=>'', 'dat'=>'', 'lts'=>'');
foreach($_ga as $_gv => $_gd)
{
	${'_'.$_gv}=$_gd;
	if(isset($_GET) && is_array($_GET) && sizeof($_GET) && isset($_GET[$_gv]))
	{
		if(is_int($_gd))
			${'_'.$_gv}=(int)$_GET[$_gv];
		else
			${'_'.$_gv}=trim($_GET[$_gv]);
	}
}

// Check existance?
$_check=false;
if(isset($_POST) && is_array($_POST) && sizeof($_POST) && isset($_POST['check']) && $_POST['check'])
	$_check=true;

// Download manual?
if(strlen($_manual))
{
/*
	$filename=trim($_GET['filename']);

	if(file_exists($filename))
	{
		// Disable output compression, or else the filesize doesn't match
		ini_set('zlib.output_compression', 'Off');

		$len = filesize($filename);

		@header('Expires: 0');
		@header('Cache-Control: store, public, must-revalidate');
		@header('Pragma: hack'); // Hack für IE
		@header('Content-Length: '.$len);
		@header('Content-Transfer-Encoding: binary');
		@header('Content-Type: application/pdf');
		@header('Content-Disposition: inline; filename="'.$file.'"');

		@readfile($filename);
		die;
	}
}
*/
	$_pa=1;
	$_type='pdf';
	$_dat=$_filename;
}

// Submitted parameter missing?
if(!strlen($_error) && (!$_pa || !strlen($_type) || !strlen($_filename) || !strlen($_dat) || !strlen($_lts)))
{
	$_error='<b>'.$_title.'</b><br />&rarr; ';
	if($GLOBALS['fxpglobals']['lang'] == 1)	// German
		$_error .= 'Mindestens ein übermittelter Parameter fehlt!';
	else
		$_error .= 'At least on submitted parameter is missing!';
}

// Does file exist?
$_filepath='';
if(!strlen($_error))
{
	$_suffix='.'.$_type;
	if(substr($_filename,-strlen($_suffix)) != $_suffix)
		$_filename .= $_suffix;
	$_filepath=urldecode(trim($_dat));
	if(!file_exists($_filepath))
		$_filepath=$GLOBALS['tmppath'].'~'.$_filepath.'.'.$_type;
	if(!file_exists($_filepath))
	{
		$_error='&quot;<b>'.$_filename.'</b>&quot;<br />&rarr; The file doesn\'t exist anymore!';
		if($GLOBALS['fxpglobals']['lang'] == 1)	// German
			$_error='&quot;<b>'.$_filename.'</b>&quot;<br />&rarr; Die Datei existiert nicht mehr!';
	}
}
//$_error='&quot;<b>'.$_filename.'</b>&quot;<br />&rarr; The file doesn\'t exist anymore!';

// Always download a ZIP/RAR file
if(!strlen($_error) && (($_type == 'zip') || ($_type == 'rar')))
	$_pa=1;

// Return existance check?
if($_check)
{
	if(strlen($_error))
		die('0|'.$_error);
	die($_pa.'|'.$_title.': '.$_filename);
}

// Return status?
if(isset($_GET) && is_array($_GET) && sizeof($_GET) && isset($_GET['rs']))
	die($_error);

// Download or Display?
if(!strlen($_error))
{
	// Disable output compression, or else the filesize doesn't match
	ini_set('zlib.output_compression', 'Off');

	// Download
	$_ce='File Transfer';
	$_cd='attachment';
	$_ct=getMIMETypes($_type);
	if(!strlen($_ct))
		$_ct='application/octet-stream';
	if($_pa != 1)	// Display
	{
		$_ce='File Display';
		if($_ct != 'application/octet-stream')
			$_cd='inline';
	}

	header('Content-Description: '.$_ce);
	header('Content-Type: '.$_ct);
	header('Content-Disposition: '.$_cd.'; filename="'.basename($_filename).'"');
	header('Expires: 0');
	header('Cache-Control: must-revalidate');
	header('Pragma: public');

	$_fs=0;
	if(($_cd == 'attachment') || ($_cd == 'download'))
	{
		$_fs=filesize($_filepath);

		header('Content-Length: '.$_fs);
		header('Content-Transfer-Encoding: binary');
	}

//echo($_ce.'<hr />'.$_cd.'<hr />'.$_ct.'<hr />$_filename='.$_filename.'<hr />$_filepath='.$_filepath.'<hr />$_fs='.$_fs.'<hr />');
	readfile($_filepath);
	die;
}

// Error
$_etitle='Error!';
if($GLOBALS['fxpglobals']['lang'] == 1)	// German
	$_etitle='Fehler!';
$_message  = '					<span style="font-size:32pt;font-weight:bolder;color:#e1001a;text-shadow:2px 2px 2px rgba(0,0,0, 0.66);">'.$_etitle.'</span><br /><br /><br /><br /><br />'.$GLOBALS['nl'];
$_message .= '					<span style="font-size:18pt;font-weight:bold;color:#fff;text-shadow:2px 2px 2px rgba(0,0,0, 0.66);">'.$_error.'</span>'.$GLOBALS['nl'];
require($GLOBALS['mainpath'].'error.inc');
die;
?>