<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : filter_db.inc                                                //
// Version     : 21.2                                                         //
// Begin       : 2020-11-02                                                   //
// Last Change : 2020-11-02                                                   //
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
 * PF 223: Filter
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$fa=getFilters();

// Save or Save as
if($_POST['Button_Speichern'] || $_POST['Button_Speichern_als'])
{
	$saveas=false;
	$unique=true;

	// Filter name + description
	if($_POST['Button_Speichern_als'])
	{
		$_POST['flt_name']='';
		$_POST['flt_desc']='';
		$id=223;
	}
	$flt_name=trim($_POST['flt_name']);
	$flt_desc=trim($_POST['flt_desc']);
	if($_POST['Button_Speichern'])
	{
		if(strtolower($flt_name) !== strtolower($fa['name']))
			$saveas=true;
		if(!$saveas && !$fa['mode'])
		{
			$flt_name='';
			$flt_desc='';
		}
//echo('$flt_name='.$flt_name.'<hr />');

		// Filter ID
		if($saveas)
			$id=(int)db_value("SELECT trans_id FROM teinstellung WHERE trans_id<0 AND mandanten_id=".$GLOBALS['fxpglobals']['client']) - 1;
		else
		{
			$id=$fa['id'];
			if(!$id)
				$id=223;
		}

		// Filter name must be unique
		if(($id < 0) && strlen($flt_name) && sizeof($fa['selects']))
		{
			$lfn=strtolower($flt_name);
			$fid=getFilterID($id);
			foreach($fa['selects'] as $sfn => $sda)
			{
				$sfn=strtolower($sfn);
				if(($lfn === $sfn) && ($sda['afilter'] !== $fid))
				{
					$unique=false;
					break;
				}
			}
		}
//fxDebug($unique,'$unique');
	}

	if(!$unique)
	{
		// 3080: Ein Filter mit diesem Namen existiert bereits! Bitte geben Sie einen anderen Namen ein.
		error_msg(3080,array('flt_name'));
		$err=true;
	}
	else
	{
		// Filter String
		$str=filter_string(true, $flt_name, $flt_desc);

		// Write filter into db
		$err=store_settings($str,$id);
//$err=true;
		if($err)
			error_msg(71,false);	// 71: Die Filtereinstellungen konnten nicht gespeichert werden!
		else
		{
			$GLOBALS['fxpglobals']['afilter']=getFilterID($id);
			$fa=getFilters(true);
			eval($fa['setting']);
			error_msg(70,false);	// 70: Die Filtereinstellungen wurden erfolgreich gespeichert und gesetzt.
		}
	}
}

// Delete
if($_POST['Button_Loeschen'])
{
	$err=delete_settings($fa['id']);
	if($err)
		error_msg(3079,false);	// 3079: Die Filtereinstellungen konnten nicht gelöscht werden!
	else
	{
		$GLOBALS['fxpglobals']['afilter']='0';
		$fa=getFilters(true);
		eval($fa['setting']);
		error_msg(3078,false);	// 3078: Die Filtereinstellungen wurden erfolgreich gelöscht und der Standardfilter neu gesetzt.
	}
}
?>