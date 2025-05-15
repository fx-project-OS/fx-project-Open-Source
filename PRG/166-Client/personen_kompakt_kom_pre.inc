<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : personen_kompakt_kom_pre.inc                                 //
// Version     : 24.1                                                         //
// Begin       : 2020-10-05                                                   //
// Last Change : 2024-03-14                                                   //
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
 * PF 166: Client etc. - Communications (will be called as an alternative program function from other person types as well)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

// Variable preparations
$com_cnt=0;
$index=true;
if(isset($GLOBALS['fxptdata']['lvalues']) && isset($GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']]))
	unset($GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']]);

$hr_nv=true;

// No frame for field
$Checkbox386_ffs=0;

// All communication mask fields
$mfelder=fxf_getMaskFields(386,false,array('Checkbox'=>0));
//fxDebug($mfelder,'$mfelder', 0);

// Restore submitted values on error
if($err && fxIsArray($_POST['Kommunikation_ID'][$GLOBALS['_maskcounter']]))
{
	foreach($_POST['Kommunikation_ID'][$GLOBALS['_maskcounter']] as $com_cnt => $kid)
	{
		if(!strlen((string)$kid))
		{
			// Empty line with top border
			fxf_maskColumnBorder($mfelder, 1, -1, $GLOBALS['_maskcounter'], $com_cnt, true);
			continue;
		}

		$cls='';
		if($GLOBALS['fxpglobals']['action'] == FXP_DELETE)
			$cls='^class_rv'.($com_cnt%2).'|';

		$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']][$com_cnt]['kommunikation_id']=$kid;
		foreach($mfelder as $fname)
		{
			$lfname=strtolower((string)$fname);
			if($lfname != 'kommunikation_id')
				$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']][$com_cnt][$lfname]=$cls.$_POST[$fname][$GLOBALS['_maskcounter']][$com_cnt];
			else
				$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']][$com_cnt][$lfname]=$_POST[$fname][$GLOBALS['_maskcounter']][$com_cnt];
		}

		if($GLOBALS['fxpglobals']['action'] != FXP_DELETE)
		{
			$cb=$cb_lst;
			if($aid < 0)
				$cb=$cb_new;
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']][$com_cnt]['checkbox']='~'.$cb;
		}
		else
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']][$com_cnt]['checkbox']='~'.fieldCheckbox('checkbox['.$GLOBALS['_maskcounter'].']['.$com_cnt.']','',$_POST['checkbox'][$GLOBALS['_maskcounter']][$com_cnt]);
	}
}
// Get values from database
else
{
	if($GLOBALS['fxpglobals']['action'] != FXP_CREATE)
	{
		$com_sql  = "SELECT k.kommunikation_id, k.kommunikationstyp, k.kommunikation_txt, k.anmerkung, w.wertetabid, w.tabwert, w.id_sprache, w.position";
		$com_sql .= " FROM kommunikation k, wertetab w";
		$com_sql .= " WHERE k.personen_id=".$_pid." AND k.mandanten_id=".$_cid;
		$com_sql .= " AND w.wertetabid=k.kommunikationstyp AND w.id_sprache=".$GLOBALS['fxpglobals']['lang']." AND w.mandanten_id IN (0,".$_cid.")";
		$com_sql .= " ORDER BY w.position, w.tabwert, k.kommunikation_txt";
		$com_arr=db_values($com_sql, '*');
fxDebug($com_arr, $com_sql, 3);

		// Display values
		if(fxIsArray($com_arr))
		{
			foreach($com_arr as $ka)
			{
				$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']][$com_cnt]=$ka;

				if($GLOBALS['fxpglobals']['action'] == FXP_DELETE)
				{
					$cls='^class_rv'.($com_cnt%2).'|';
					foreach($mfelder as $fname)
					{
						$lfname=strtolower((string)$fname);
						if($lfname != 'kommunikation_id')
							$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']][$com_cnt][$lfname]=$cls.$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']][$com_cnt][$lfname];
					}
					$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']][$com_cnt]['checkbox']='~'.fieldCheckbox('checkbox['.$GLOBALS['_maskcounter'].']['.$com_cnt.']','','');
				}
				else
					$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']][$com_cnt]['checkbox']='~'.$cb_lst;

				$com_cnt++;
			}

			// Empty line with top border
			fxf_maskColumnBorder($mfelder, 1, -1, $GLOBALS['_maskcounter'], $com_cnt, true);
			$com_cnt++;
		}
	}
	// Creation of new values (even in change mode)
	if(($GLOBALS['fxpglobals']['action'] == FXP_CREATE) || ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE))
	{
		for($j=0; $j<$GLOBALS['fxpglobals']['settings']['listenneu']; $j++)
		{
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']][$com_cnt]['kommunikation_id']=-1;
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']][$com_cnt]['checkbox']='~'.$cb_new;
			$com_cnt++;
		}

		// Empty line with top border
		fxf_maskColumnBorder($mfelder, 1, -1, $GLOBALS['_maskcounter'], $com_cnt, true);
		$com_cnt++;
	}
}
if(!$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']])
	$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']]=true;
?>