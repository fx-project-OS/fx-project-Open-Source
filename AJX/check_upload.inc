<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : check_upload.inc                                             //
// Version     : 21.2                                                         //
// Begin       : 2020-08-20                                                   //
// Last Change : 2018-07-30                                                   //
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
 * ???FILE???
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

session_id($_GET['sesstoid']);
session_start();

$k=ini_get('session.upload_progress.prefix').$_GET['fieldname'];
$up=(int)$_GET['up'];
$lng=(int)$_GET['lng'];

// fc: filecount
$fc=0;
if(isset($_GET['fc']))
	$fc=(int)$_GET['fc'];

/*
	echo("session_id=".session_id()."\n\$k=".$k."\n\$up=".$up."\n");
	print_r($_SESSION);
	exit;
*/

uploadProgressDebug(0);

$pc=(int)$up;
$rm='';
$fn='';
$cl=0;
$bp=0;

$esc=0;
$esm=50;
/*
	while(($esc < $esm) && (empty($_FILES) || empty($_SESSION) || empty($_SESSION[$k])))
	{
		uploadProgressDebug(0);
		$esc++;
		usleep(100000);
	}
*/

if($esc >= $esm)
	$pc=-2;
else if(!empty($_SESSION[$k]) && !$_SESSION[$k]['done'])
{
	if($pc == -1)
		$_SESSION[$k]['cancel_upload']=true;
	else
	{
		$af=0;
		if($fc)
			$sf=$fc;
		else
			$sf=sizeof($_SESSION[$k]['files']);
		for($i=0; $i<sizeof($_SESSION[$k]['files']); $i++)
		{
			$af=$i+1;
			$fn=$_SESSION[$k]['files'][$i]['name'];
			if(!$_SESSION[$k]['files'][$i]['done'])
				break;
		}
		$fn=$af.'/'.$sf.': '.$fn;
		$cl=(int)$_SESSION[$k]['content_length'];
		$bp=(int)$_SESSION[$k]['bytes_processed'];
		if($bp < $cl)
			$pc=max($pc,ceil($bp*100/$cl));
		else
			$pc=100;
	}
}
else
	$pc=100;

if($pc == -2)
{
	if($lng == 1)
		$rm='Upload konnte nicht gestartet werden (Datei zu gross?)';
	else
		$rm='Upload could not be started (File too big?)';
}
else if($pc == -1)
{
	if($lng == 1)
		$rm='Upload abgebrochen!';
	else
		$rm='Upload canceled!';
}
else if($pc >= 100)
{
	if($lng == 1)
		$rm='Upload fertig. Datei wird bereitgestellt...';
	else
		$rm='Upload done. File is being prepared...';
}
else if($pc >= 0)
	$rm=uploadBytes($bp,$lng).' / '.uploadBytes($cl,$lng);

$rs=$pc.'*'.$rm.'*'.$fn.'*'.$cl.'*'.$bp;

uploadProgressDebug(1);

die($rs);


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * ???FUNCTION???
 *
 * @param integer $md - Optional parameter (default = 0): ???PARAMETER???
 */
function uploadProgressDebug($md=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return;

	$content  = "________________________________________\r\n".date('Y-m-d H:i:s',time())."\r\n========================================\r\n";
	$content .= "sesstoid=".$_GET['sesstoid'].", fieldname=".$_GET['fieldname'].", esc=".$GLOBALS['esc'].", up=".$GLOBALS['up'].", fc=".$GLOBALS['fc'].", lng=".$GLOBALS['lng']."\r\n\r\n";
	if(!$md)
		$content .= print_r($_SESSION,true)."\r\n\r\n";
	else
		$content .= "-> ".$GLOBALS['rs']."\r\n\r\n";

	fxSave($GLOBALS['tmppath'].'_upload_progress_debug.txt', $content, false);
}

/**
 * ???FUNCTION???
 *
 * @param various $b   - Mandatory parameter: ???PARAMETER???
 * @param various $lng - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function uploadBytes($b,$lng)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$l=strlen($b);
	$s=$b;
	$u='B';

	if(($l > 3) && ($l < 7))
	{
		$s=round($b/1024,2);
		$u='KB';
	}
	else if(($l > 6) && ($l < 10))
	{
		$s=round($b/1048576,2);
		$u='MB';
	}
	else if($l > 9)
	{
		$s=round($b/1073741824,2);
		$u='GB';
	}

	$f='';
	$se=explode('.',$s);
	$c=0;
	for($i=strlen($se[0])-1; $i>=0; $i--)
	{
		$c++;
		if($c == 3)
		{
			$c=0;
			if($lng == 1)
				$f = '.'.$f;
			else
				$f = ','.$f;
		}
		$f = substr($se[0],$i,1).$f;
	}
	if(sizeof($se) > 1)
	{
		if($lng == 1)
			$f .= ',';
		else
			$f .= '.';
		$f .= substr($se[1].'00',0,2);
	}

	return $f.' '.$u;
}
?>