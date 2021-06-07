<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : loader.php                                                   //
// Version     : 21.1                                                         //
// Begin       : 2020-08-28                                                   //
// Last Change : 2021-04-19                                                   //
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
 * Loader program to check if a requested PHP/INC/TXT file exsists - if it does, this program is loaded and executed, else either the error handler or the default index program is loaded and executed
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

$GLOBALS['__loaded_'.basename(__FILE__)]=true;


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

// User must be logged in?
$__valid_call=false;
if(fxIsArray($GLOBALS['fxpglobals']) && isset($GLOBALS['fxpglobals']['user']) && ($GLOBALS['fxpglobals']['user'] > 0) && isset($GLOBALS['fxpglobals']['client']) && ($GLOBALS['fxpglobals']['client'] > 0))
	$__valid_call=true;

// Get program that should be loaded
//fxDebug('_GET');
$__cpnm='';
$__cpnp='';
if(fxIsArray($_GET) && isset($_GET['url']))
{
	$__cpnm=trim($_GET['url']);
	$__qaa=array('?', '&');
	foreach($__qaa as $__qa)
	{
		$__qap=strpos($__cpnm,$__qa);
		if($__qap !== false)
			$__cpnm=trim(substr($__cpnm,0,$__qap));
	}
	$__cpnp=basename($__cpnm);
}
//fxDebug(array('$__cpnm'=>$__cpnm, '$__cpnp'=>$__cpnp),'GET', 0);

// Check if program exists (if it does, get name and type)
$__fpnm='';
$__fpst='';
if(strlen($__cpnm) && strlen($GLOBALS['locstoid']) && strlen($GLOBALS['sesstoid']) && isset($GLOBALS['_phpfiles']) && strlen($GLOBALS['_phpfiles']) && file_exists($GLOBALS['_phpfiles']))
{
	require($GLOBALS['_phpfiles']);
//fxDebug($ppa,'$ppa', 0);
	if(fxIsArray($ppa))
	{
		$__vcpa=array();
		if(!$__valid_call)
		{
			if(($__cpnm == 'check_patch.inc') && fxIsArray($_GET) && isset($_GET['lcnt']) && ((int)$_GET['lcnt'] == 0))
				$__vcpa[$__cpnm]='AJX';
			if(($__cpnm == 'timestamp.inc') && fxIsArray($_GET) && isset($_GET['lcnt']) && ((int)$_GET['lcnt'] > 0))
				$__vcpa[$__cpnm]='AJX';
			if(($__cpnm == 'worker.inc') && fxIsArray($_GET) && isset($_GET['lcnt']) && ((int)$_GET['lcnt'] > 1))
				$__vcpa[$__cpnm]='SYS';
			if((($__cpnm == 'check_update.inc') || ($__cpnm == 'popup_hlp.inc')) && fxIsArray($_GET) && isset($_GET['lcnt']) && ((int)$_GET['lcnt'] > 2))
				$__vcpa[$__cpnm]='AJX';
			if(($__cpnm == '021_ini.inc') && fxIsArray($_POST) && isset($_POST['mode']))
				$__vcpa[$__cpnm]='SPP';
//fxDebug($__vcpa,'$__vcpa', 0);
		}
		foreach($ppa as $phpfilename => $pa)
		{
			if(($pa['filename'] === $__cpnm) && ($__valid_call || !strlen($pa['filepath']) || ($pa['filepath'] == 'APP') || ($pa['filepath'] == 'INT') || isset($__vcpa[$pa['filename']])))
			{
				$__fpnm=$phpfilename;
				$__dp=strrpos($__fpnm,'.');
				if($__dp)
					$__fpst=strtolower(substr($__fpnm,$__dp+1));
				break;
			}
		}
	}
}
//fxDebug(array('$__fpnm'=>$__fpnm, '$__fpst'=>$__fpst),'CHECK', 0);

// Requested program found (and is valid: name and type)?
$__vta=array('php'=>true, 'inc'=>true, 'txt'=>true);
if(strlen($__fpnm) && strlen($__fpst) && isset($__vta[$__fpst]))
{
	require($GLOBALS['mainpath'].$__fpnm); die;
}

// Requested program not found?
if(strlen($__cpnm))
{
//fxDebug($GLOBALS['__server_array'],'$GLOBALS[\'__server_array\']', 0);
	require($GLOBALS['mainpath'].'error.inc'); die;
}

// Default
//fxDebug($GLOBALS['__server_array'],'$GLOBALS[\'__server_array\']', 0); die;
require($GLOBALS['mainpath'].'index.php');
?>