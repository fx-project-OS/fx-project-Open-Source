<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : includer.inc                                                 //
// Version     : 21.2                                                         //
// Begin       : 2020-09-02                                                   //
// Last Change : 2021-03-30                                                   //
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
 * Includer program to check if requested files exsist.
 * - If a requested existing file has not been already included it will be included now.
 * - If a requested existing file has already been included it will be skipped.
 *
 * The to be included files have to be added to the array $GLOBALS['__includer'] before calling this file.
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
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

// Check programs that should be included
if(fxIsArray($GLOBALS['__includer']))
{
	// Debug includer?
	$__idl='';
	if(!isset($__idb))
	{
		$__idb=0;
		//$__idb=-1;	// Log into error_log
		//$__idb=1;		// Log on screen
	}

	// Check program function pathes?
	if(fxIsArray($_GET) && isset($_GET['__itr']))
		$__itr=$_GET['__itr'];
	if(!isset($__itr))
	{
		if(fxIsArray($GLOBALS['fxpglobals']) && isset($GLOBALS['fxpglobals']['tr']) && $GLOBALS['fxpglobals']['tr'])
			$__itr=$GLOBALS['fxpglobals']['tr'];
		else
			$__itr=array();
	}

	// Pathes to check
	$__ipa=fxf_fn_reqFilename('', $__itr);

	// Check for file in pathes
	$__pfxa=array('.inc', '.php', '.pfi');
	foreach($GLOBALS['__includer'] as $__ifname => $__dummy)
	{
		$__iok=0;
		if($__idb)
			$__idl .= 'Check for: &quot;'.$__ifname.'&quot;<br />';
		if(!isset($GLOBALS['__loaded_'.$__ifname]))
		{
			$__inc='';
			foreach($__ipa as $__ip)
			{
				if($__idb)
					$__idl .= ' ... in directory: &quot;'.$__ip.'&quot;<br />';
				foreach($__pfxa as $__pfx)
				{
					$__fn=$__ifname;
					if(substr($__fn,-4) != $__pfx)
						$__fn .= $__pfx;
					if($__idb)
						$__idl .= ' ...... with prefix: &quot;'.$__pfx.'&quot;<br />';

					$__ipfn=$__ip.$__fn;
//echo(__FILE__.' ('.__LINE__.') - Check for &quot;'.$__ipfn.'&quot; ...<br />');
					if(file_exists($__ipfn))
					{
						$__inc=$__ipfn;
						break;
					}
				}
			}

			if(strlen($__inc))
			{
				$GLOBALS['__loaded_'.$__ifname]=true;
				if($__idb)
					$__idl .= ' $rarr; <b>Found &quot;'.$__inc.'&quot;!</b> &rarr; Include<hr />';
				$__iok=2;
				include($__inc);
			}
			else if($__idb)
				$__idl .= ' $rarr; <b>Not found!<hr />';
		}
		else if($__idb)
		{
			$__iok=1;
			$__idl .= ' &rarr; <b>Already included!</b><hr />';
		}
//echo(__FILE__.' ('.__LINE__.') - Status for &quot;'.$__ifname.'&quot;: '.$__iok.'<br />');
	}

	if($__idb && strlen($__idl))
	{
		if($__idb > 0)
			echo($__idl);
		else
			error_log($__idl);
	}
}
?>