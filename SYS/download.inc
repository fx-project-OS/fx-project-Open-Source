<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : download.inc                                                 //
// Version     : 21.2                                                         //
// Begin       : 2020-08-20                                                   //
// Last Change : 2021-04-12                                                   //
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
 * Download a document or a file
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

set_time_limit(0);
// Disable output compression, or else the filesize doesn't match
ini_set('zlib.output_compression', 'Off');

if(is_array($_GET) && isset($_GET['doctr']) && isset($_GET['docidx']))
{
	$idx=$_GET['docidx'];

	$GLOBALS['_form']='';
	fxSession('t','l');

	$f_exists=false;
	$ret_array=false;
	$filename=$GLOBALS['fxpglobals']['settings']['dok_archiv'].$GLOBALS['fxpglobals']['client'].'/'.$GLOBALS['fxptdata']['backup']['lwidx'][$idx]['dnm'];
	if(file_exists($filename))
		$f_exists = true;

	if($GLOBALS['fxptdata']['backup']['lwidx'][$idx]['usr'] && $f_exists)
	{
		// Evtl. vorhanden Dokumentobjekte zerstören und ein neues erstellen
		if(isset($Dok) && is_object($Dok))
			$Dok->suizid($Dok);
		$Dok=new fcDoc('Dok', $GLOBALS['fxptdata']['backup']['lwidx'][$idx]['art'], $GLOBALS['fxptdata']['backup']['lwidx'][$idx]['elt'], $GLOBALS['fxptdata']['backup']['lwidx'][$idx]['did']);
		// Log-Datei schreiben: 1=DOWNLOAD
		if(isset($Dok) && is_object($Dok))
			@$Dok->schreibe_log(1);

		// der eigentliche Download
		@set_time_limit(0);

		if(is_array($ret_array))
			$len=strlen($ret);
		else
			$len=filesize($filename);

		@header('Expires: 0');
		@header('Cache-Control: store, public, must-revalidate');
		@header('Pragma: hack'); // Hack für IE
		@header('Content-Type: application/octet-stream');
		@header('Content-Length: '.$len);
		@header('Content-Disposition: attachment; filename="'.$GLOBALS['fxptdata']['backup']['lwidx'][$idx]['onm'].'"'); // Dateinamen im Download-Requester vorgeben (originaler Dateiname)
		@header('Content-Transfer-Encoding: binary');

		if(is_array($ret_array))
			echo($ret);
		else
			@readfile($filename);
		exit;
	}
}
else if(is_array($_GET) && isset($_GET['fn']))
{
	$filepath=$_GET['fn'];
	if(substr($filepath,0,1) == '/')
		$filepath=$GLOBALS['mainpath'].substr($filepath,1);

	if(strlen($filepath) && file_exists($filepath))
	{
		$filename=$filepath;
		while(true)
		{
			$sp=strpos($filename,'/');
			if($sp === false)
				break;
			$filename=substr($filename,$sp+1);
		}
		if(is_array($_GET) && isset($_GET['tp']))
		{
			$type=$_GET['tp'];
			if(substr($filename,-4) == '.tmp')
				$filename=substr($filename,0,-3).$type;
		}

		$len=filesize($filepath);
		@header('Expires: 0');
		@header('Cache-Control: store, public, must-revalidate');
		@header('Pragma: hack'); // Hack für IE
		@header('Content-Type: application/octet-stream');
		@header('Content-Length: '.$len);
		@header('Content-Disposition: attachment; filename="'.$filename.'"'); // Dateinamen im Download-Requester vorgeben (originaler Dateiname)
		@header('Content-Transfer-Encoding: binary');

		@readfile($filepath);
	}
}
?>