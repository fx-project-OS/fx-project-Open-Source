<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : history_ini.inc                                              //
// Version     : 24.1                                                         //
// Begin       : 2022-04-28                                                   //
// Last Change : 2024-10-09                                                   //
// Author      : FeRox Management Consulting GmbH & Co. KG                    //
//               Adolf-Langer-Weg 11a, D-94036 Passau (Germany)               //
//               https://www.ferox.de - info@ferox.de                         //
// License     : GNU-GPL v3 (https://opensource.org/licenses/GPL-3.0)         //
// -------------------------------------------------------------------------- //
// fx-project - An open source PHP Project Managament Software                //
// Copyright  ? FeRox Management Consulting GmbH & Co. KG                     //
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
 * PF 283: History Management
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

// Error message
$emsg='';

// Filename array
$fna=array();

// Newline replacement array
$nlrs='|<*>|';
$nlra=array("\r\n"=>$nlrs, "\r"=>$nlrs, "\n"=>$nlrs);

// Should we use dms
if(!class_exists('fxpDoc'))
{
	$GLOBALS['__includer']['dms']=true;
	require('includer.inc');
}
$use_dms=false;
if(!strlen((string)$emsg) && isset($GLOBALS['sppdir'][110]) && function_exists('fxsppCheckEBTR') && fxsppCheckEBTR(110) && class_exists('fxpDoc'))
	$use_dms=true;
//$use_dms=false;
//fxDebug($use_dms,'$use_dms', 0);

// Create client folders if they dont exist already
$cfa=array('cdatpath'=>$GLOBALS['datpath'].$GLOBALS['fxpglobals']['client'].'/', 'cdmspath'=>str_replace('\\', '/', $GLOBALS['fxpglobals']['settings']['dok_archiv']));
if(substr((string)$cfa['cdmspath'],-1) != '/')
	$cfa['cdmspath'] .= '/';
$cfa['cdmspath'] .= $GLOBALS['fxpglobals']['client'].'/';
//fxDebug($cfa,'$cfa', 0);
foreach($cfa as $cn => $cp)
{
	$pe=fxCreatePath($cp);
	if(strlen((string)$pe))	// Error
	{
		// The client folder $var1 could not be created or is write protected!
		error_msg(3388,false, '[<b>'.$pe.'</b>]');
		$err=true;
	}
	else
	{
		$GLOBALS[$cn]=$cp;
		@chmod($cp, 0777);
		@chown($cp, 'nobody');
	}
}


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * Get SQL extension for history table exceptions
 *
 * @param string $tn - Mandatory parameter:  Table name (in lowercase)
 *
 * @return SQL extension string or empty string
 */
function fxHistoryTableExceptions($tn)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$exs="";
	// ...Contract data
	if($tn == 'madaten_history')
		$exs=" AND aktion NOT IN (2,3)";
	// ...Carry start values
	else if($tn == 'zeitdaten_history')
		$exs=" AND (zeitart<>".FXP_TE_CARRY_START." OR aktion<>2)";

	return $exs;
}
?>