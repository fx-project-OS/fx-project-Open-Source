<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : personen_kompakt_adr_pre.inc                                 //
// Version     : 21.1                                                         //
// Begin       : 2020-10-05                                                   //
// Last Change : 2020-10-05                                                   //
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
 * PF 166: Client etc. - Addresses (will be called as an alternative program function from other person types as well)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

// Variable preparations
$adrcnt=0;
$index=true;
if(isset($GLOBALS['fxptdata']['lvalues']) && isset($GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']]))
	unset($GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']]);

$hr_nv=true;

// No frame for field
$Checkbox384_ffs=0;

// All address mask fields
$mfelder=fxf_getMaskFields(384,false,array('Checkbox'=>0));
//fxDebug($mfelder,'$mfelder', 0);

// Restore submitted values on error
if($err && fxIsArray($_POST['Adressen_ID'][$GLOBALS['_maskcounter']]))
{
	foreach($_POST['Adressen_ID'][$GLOBALS['_maskcounter']] as $adrcnt => $aid)
	{
		if(!strlen($aid))
		{
			// Empty line with top border
			fxf_maskColumnBorder($mfelder, 1, -1, $GLOBALS['_maskcounter'], $adrcnt, true);
			continue;
		}

		$cls='';
		if($GLOBALS['fxpglobals']['action'] == FXP_DELETE)
			$cls='^class_rv'.($adrcnt%2).'|';

		foreach($mfelder as $mfk => $fname)
		{
			if($mfk)
			{
				$lfname=strtolower($fname);
				if($lfname != 'adressen_id')
					$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']][$adrcnt][$lfname]=$cls.$_POST[$fname][$GLOBALS['_maskcounter']][$adrcnt];
				else
					$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']][$adrcnt][$lfname]=$_POST[$fname][$GLOBALS['_maskcounter']][$adrcnt];
			}
		}

		if($GLOBALS['fxpglobals']['action'] != FXP_DELETE)
		{
			$cb=$cb_lst;
			if($aid < 0)
				$cb=$cb_new;
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']][$adrcnt]['checkbox']='~'.$cb;
		}
		else
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']][$adrcnt]['checkbox']='~'.fieldCheckbox('checkbox['.$GLOBALS['_maskcounter'].']['.$adrcnt.']','',$_POST['checkbox'][$GLOBALS['_maskcounter']][$adrcnt]);
	}
}
// Get values from database
else
{
	if($GLOBALS['fxpglobals']['action'] != FXP_CREATE)
	{
		$adr_sql  = "SELECT a.adressen_id, a.adresstyp, a.staat, a.strasse, a.landkz, a.plz, a.ort, w.wertetabid, w.tabwert, w.id_sprache, w.position";
		$adr_sql .= " FROM adressen a, wertetab w";
		$adr_sql .= " WHERE a.personen_id=".$_pid." AND a.mandanten_id=".$_cid;
		$adr_sql .= " AND w.wertetabid=a.adresstyp AND w.id_sprache=".$GLOBALS['fxpglobals']['lang']." AND w.mandanten_id IN (0,".$_cid.")";
		$adr_sql .= " ORDER BY w.position, w.tabwert, a.plz";
		$adr_arr=db_values($adr_sql, '*');
fxDebug($adr_arr, $adr_sql, 3);

		// Display values
		if(fxIsArray($adr_arr))
		{
			foreach($adr_arr as $aa)
			{
				$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']][$adrcnt]=$aa;

				if($GLOBALS['fxpglobals']['action'] == FXP_DELETE)
				{
					$cls='^class_rv'.($adrcnt%2).'|';
					foreach($mfelder as $mfk => $fname)
					{
						if($mfk)
						{
							$lfname=strtolower($fname);
							if($lfname != 'adressen_id')
								$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']][$adrcnt][$lfname]=$cls.$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']][$adrcnt][$lfname];
						}
					}
					$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']][$adrcnt]['checkbox']='~'.fieldCheckbox('checkbox['.$GLOBALS['_maskcounter'].']['.$adrcnt.']','','');
				}
				else
					$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']][$adrcnt]['checkbox']='~'.$cb_lst;

				$adrcnt++;
			}

			// Empty line with top border
			fxf_maskColumnBorder($mfelder, 1, -1, $GLOBALS['_maskcounter'], $adrcnt, true);
			$adrcnt++;
		}
	}
	// Creation of new values (even in change mode)
	if(($GLOBALS['fxpglobals']['action'] == FXP_CREATE) || ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE))
	{
		for($j=0; $j<$GLOBALS['fxpglobals']['settings']['listenneu']; $j++)
		{
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']][$adrcnt]['adressen_id']=-1;
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']][$adrcnt]['checkbox']='~'.$cb_new;
			$adrcnt++;
		}

		// Empty line with top border
		fxf_maskColumnBorder($mfelder, 1, -1, $GLOBALS['_maskcounter'], $adrcnt, true);
		$adrcnt++;
	}
}
if(!$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']])
	$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']]=true;
?>