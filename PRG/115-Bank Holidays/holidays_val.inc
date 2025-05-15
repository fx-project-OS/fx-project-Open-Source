<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : feiertage_sind_val.inc                                       //
// Version     : 24.1                                                         //
// Begin       : 2020-09-24                                                   //
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
 * PF 115: Bank Holidays - File for validating bank holidays
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

if(fxIsArray($_POST) && isset($_POST['Button_Multi']))
{
//fxDebug('_POST');
	// Counter for ticked checkboxes
	$cc=0;

	// Array key
	$ak=-1;
	if($_POST['nv_text1'])
	{
		reset($_POST['nv_text1']);
		$ak=key($_POST['nv_text1']);
	}
	if($ak >= 0)
	{
		foreach($_POST['nv_text1'][$ak] as $l => $ht)
		{
			$val_zeilennummer=$l;

			if(strlen((string)$ht))
			{
				// Hidden values
				$a=explode($GLOBALS['_divstr'][0], $ht);
				$ha=array('hid'=>(int)$a[0], 'nid'=>(int)$a[1], 'calc'=>trim((string)$a[2]), 'type'=>(int)$a[3], 'hcheck'=>(int)$a[4], 'hdate'=>trim((string)$a[5]), 'hname'=>trim((string)$a[6]), 'hlength'=>(int)$a[7], 'check'=>(int)$a[4], 'date'=>trim((string)$a[5]), 'name'=>trim((string)$a[6]), 'length'=>(int)$a[7]);
//fxDebug($ha,'$ha: $l='.$l, 0);

				// Submitted values
				$hva[$l]=$ha;
				// ...(Checkbox)
				$mandatory=0;
				if(($GLOBALS['fxpglobals']['action'] == FXP_CHANGE) || (fxIsArray($_POST['Checkbox']) && fxIsArray($_POST['Checkbox'][$ak]) && isset($_POST['Checkbox'][$ak][$l]) && $_POST['Checkbox'][$ak][$l]))
				{
					$hva[$l]['check']=1;
					if(($GLOBALS['fxpglobals']['action'] == FXP_CREATE) || ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE))
						$mandatory=1;
					$cc++;
				}
				// ...(Date)
				$derr=false;
				if(fxIsArray($_POST['Datum']) && fxIsArray($_POST['Datum'][$ak]) && isset($_POST['Datum'][$ak][$l]))
				{
					$hva[$l]['date']=validate_entry($_POST['Datum'][$ak][$l], $derr, 'date', $mandatory);
					if($derr)
					{
						error_msg($derr, false, 'Datum');
						$err=true;
					}
					// ...Check if date is outside of selected year
					else if(strlen((string)$hva[$l]['date']))
					{
						$hva[$l]['date']=substr((string)$hva[$l]['date'],0,8);
						$di8=(int)$hva[$l]['date']; $mi8=(int)$year.'0101'; $mx8=(int)$year.'1231';
						// The date [$var1] is outside the valid area: $var2
						if(($di8 < $mi8) || ($di8 > $mx8))
						{
							$derr=1989;
							error_msg($derr, array('Datum'), '<b>'.get_entry($di8,'date').'</b>', get_entry($mi8,'date').'-'.get_entry($mx8,'date'));
							$err=true;
						}
					}
				}
				// ...(Holiday = Name)
				$terr=false;
				if(fxIsArray($_POST['Name_Feiertag']) && fxIsArray($_POST['Name_Feiertag'][$ak]) && isset($_POST['Name_Feiertag'][$ak][$l]))
				{
					$hva[$l]['name']=validate_entry($_POST['Name_Feiertag'][$ak][$l], $terr, 'text', $mandatory);
					if($terr)
					{
						error_msg($terr, false, 'Name_Feiertag');
						$err=true;
					}
				}
				// ...(Half Day = Length)
				if(fxIsArray($_POST['halbtaegig']) && fxIsArray($_POST['halbtaegig'][$ak]) && isset($_POST['halbtaegig'][$ak][$l]))
					$hva[$l]['length']=(int)$_POST['halbtaegig'][$ak][$l];

				// No validation error -> Determine database action
				$hva[$l]['action']='';
				if(!$derr && !$terr && $hva[$l]['check'])
				{
					// ...Delete?
					if(($GLOBALS['fxpglobals']['action'] == FXP_DELETE) && $hva[$l]['hid'])
						$hva[$l]['action']='delete';
					// ...Update?
					else if(($GLOBALS['fxpglobals']['action'] == FXP_CHANGE) && $hva[$l]['hid'] && (($hva[$l]['date'] != $hva[$l]['hdate'])  || ($hva[$l]['name'] != $hva[$l]['hname'])  || ($hva[$l]['length'] != $hva[$l]['hlength'])))
						$hva[$l]['action']='update';
					// ...Insert?
					else if(($GLOBALS['fxpglobals']['action'] == FXP_CREATE) && !$hva[$l]['hid'])
						$hva[$l]['action']='insert';

					if(!strlen((string)$hva[$l]['calc']))
						$hva[$l]['calc']=substr((string)$hva[$l]['date'],4,4);
				}
			}
			else if(fxIsArray($_POST['Name_Feiertag']) && fxIsArray($_POST['Name_Feiertag'][$ak]) && isset($_POST['Name_Feiertag'][$ak][$l]))
				$hva[$l]=trim((string)$_POST['Name_Feiertag'][$ak][$l]);
			else
				$hva[$l]='';
//fxDebug($hva[$l],'$hva['.$l.']', 0);
		}
	}

	// There were no entries ticked!
	if(!$err && !$cc)
	{
		error_msg(1642);
		$err=true;
	}
//$err=true;
}
?>