<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : check_update.inc                                             //
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
 * Write update progress
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$GLOBALS['nosession']=true;

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
	{
		require($__prg);
	}
	else
		die('<div style="padding:8px;"><h1 style="padding:8px;color:red;background:#fff;border:2px solid red;box-shadow:4px 4px 4px rgba(0,0,0, 0.5);">fx-project - ERROR: Necessary file &quot;'.$__pnm.'&quot; not found!</h1></div>');
}

$pcnt=(int)$_GET['pcnt'];
$pp=(int)$_GET['pp'];
$lng=(int)$_GET['lng'];

$pm='';
$rm='';

$log_filename=$GLOBALS['logpath'].'progress.log';
if(file_exists($log_filename))
{
	$lc=0;
	$fp=false;
	while($lc < 20)
	{
		$fp=fopen($log_filename, 'r');
		if($fp !== false)
			break;
		usleep(100000);
		$lc++;
	}
	if($fp !== false)
	{
		$rm=fread($fp, filesize($log_filename));
		fclose($fp);
	}
}

if(($rm !== false) && strlen($rm))
{
	$re=explode('|',$rm);
	if((string)$re[0] !== (string)((int)$re[0]))
		$rm=$pp.'|'.$rm;
	else
	{
		$progress=(int)$re[0];

		$message='';
		if(sizeof($re) > 1)
			$message=trim($re[1]);

		$pmax=0;
		if(sizeof($re) > 2)
			$pmax=(int)$re[2];

		if(sizeof($re) > 3)
		{
			$tline=(int)$re[3];
			$tfile=trim($re[4]);
		}
		else
		{
			$tline=__LINE__;
			$tfile=__FILE__;
		}

		if(($progress == $pp) && ($progress < $pmax))
		{
			$progress++;
			plog($progress, $message, $pmax, $tline, $tfile);
		}

		$rm=$progress.'|'.$message.'|'.$pmax.'|'.$tline.'|'.$tfile;
	}
	exit($rm);
}
exit($pp.'||'.__LINE__.'|'.__FILE__);
?>