<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : faezuordnung_val.inc                                         //
// Version     : 21.2                                                         //
// Begin       : 2020-10-20                                                   //
// Last Change : 2020-10-20                                                   //
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
 * PF 22: Employee's Roles
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$aa=array();
$ea=array();

$pid=0;
if(fxIsArray($_POST) && isset($_POST['Personen_ID']))
	$pid=$_POST['Personen_ID'];

//fxDebug('_POST');
if(!$err && $pid && $_POST['Button_Multi'] && fxIsArray($_POST['Dummy']))
{
	// Key
	reset($_POST['Dummy']);
	$mc=key($_POST['Dummy']);

	foreach($_POST['Dummy'][$mc] as $l => $d)
	{
		if(strlen($d))
		{
			$val_zeilennummer=$l;
			$de=false;
			$ve=false;

			$gi=(int)$_POST['MaArt_ID'][$mc][$l];
			$si=(int)$_POST['MaFaehigkeit_ID'][$mc][$l];

			$da=explode('|',$d);
			$oi=(int)$da[0];
			$oe=validate_entry($da[1],$de,'dezimal');
			$or=(int)$da[2];
			$oc=trim($da[3]);
			$gn=trim($da[4]);
			$sn=trim($da[5]);
			if(strlen($oc))
				$oc="'".convert_string($oc,'todb')."'";
			else
				$oc='NULL';

			$cb=0;
			if(isset($_POST['Checkbox'][$mc][$l]) && $_POST['Checkbox'][$mc][$l])
				$cb=1;

			$ne=trim($_POST['Dauer'][$mc][$l]);
			$nr=(int)$_POST['Bewertung'][$mc][$l];
			$nc=trim($_POST['Kommentar'][$mc][$l]);
			if(strlen($nc))
				$nc="'".convert_string($nc,'todb')."'";
			else
				$nc='NULL';

			// Experience
			if(!strlen($ne) && ($nr || ($nc != 'NULL')))
			{
				error_msg(751,false,'Dauer');	// 751: Das Feld $var1 darf nicht leer sein. Bitte geben Sie eine Zahl ein!
				$err=true;
				$ve=true;
			}
			if(strlen($ne))
			{
				$ne=validate_entry($ne,$ve,'dezimal');
				if(!$ve)
				{
					if((substr($ne,0,1) == '-') || (substr($ne,0,4) == '0.00')) // negative or null?
						$ve=true;
				}
				$_POST['Dauer'][$mc][$l]=$ne;
				if($ve)
				{
					error_msg(1524,false,'Dauer');	// 1524: Bitte geben Sie in das Feld $var1 eine gültige positive Zahl ein!
					$err=true;
				}
			}
//echo('<b>['.$l.']</b><br>$oi='.$oi.', $oe='.$oe.', $or='.$or.', $oc='.$oc.'<br>$ne='.$ne.', $nr='.$nr.', $nc='.$nc.'<hr>');

			if(!$ve)
			{
				// Delete
				if($oi && $cb)
				{
					if(!fxIsArray($aa['del']))
						$aa['del']=array();
					$aa['del'][$l]=array('id'=>$oi, 'gi'=>$gi, 'si'=>$si, 'gn'=>$gn, 'sn'=>$sn);
				}
				// Update
				if($oi && strlen($ne) && (($ne != $oe) || ($nr != $or) || ($nc != $oc)))
				{
					if(!fxIsArray($aa['upd']))
						$aa['upd']=array();
					$aa['upd'][$l]=array('id'=>$oi, 'gi'=>$gi, 'si'=>$si, 'experience'=>$ne, 'rating'=>$nr, 'comment'=>$nc, 'gn'=>$gn, 'sn'=>$sn);
				}
				// Insert
				if(!$oi && strlen($ne))
				{
					if(!fxIsArray($aa['ins']))
						$aa['ins']=array();
					$aa['ins'][$l]=array('gi'=>$gi, 'si'=>$si, 'experience'=>$ne, 'rating'=>$nr, 'comment'=>$nc, 'gn'=>$gn, 'sn'=>$sn);
				}
			}
			else
				$ea[$l]=true;
		}
	}
}
?>