<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : rechte_trans_db.inc                                          //
// Version     : 24.1                                                         //
// Begin       : 2020-10-12                                                   //
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
 * PF 56: Profile Program Assignment - Database actions
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

if(!$err && $db_update && fxIsArray($_POST) && isset($_POST['Button_Speichern']))
{
//fxDebug('_POST');

	$ins=0;
	$upd=0;
	$del=0;

	// Analyse POST
	$mc=2;
	if(isset($_POST['mc']))
		$mc=(int)$_POST['mc'];

	$pa=array();
	foreach($_POST as $pkey => $pvar)
	{
		if(substr((string)$pkey,0,5) == 'info_')
		{
			$fieldname=substr((string)$pkey,5);
			if(!isset($pa[$fieldname]))
			{
				if(substr((string)$fieldname,0,1) == 'm')
					$pid=-1*(int)substr((string)$fieldname,1);
				else
					$pid=(int)$fieldname;
				$pa[$fieldname]=$pid;
			}
		}
	}
fxDebug($pa,'$pa', 3);

	if(fxIsArray($pa))
	{
		foreach($pa as $pid => $id)
		{
			if(fxIsArray($_POST['info_'.$pid]) && fxIsArray($_POST['info_'.$pid][$mc]))
			{
				// Check all lines
				foreach($_POST['info_'.$pid][$mc] as $l => $ov)
				{
					$ov=trim((string)$ov);
					if(strlen((string)$ov))
					{
						// Old values
						$ova=explode('_',$ov);
						$ov_tr=(int)$ova[0];									// Program function id
						$ov_pa=(int)$ova[1];									// Possible action bits
						$ov_sa=(int)$ova[2];									// Set action bits
						$ov_id=(int)$ova[3];									// Table entry ID
						$ov_tn=$arr['bgr'][$id]['nam'].': <b>'.$ova[4].'</b>';	// Program function name

						// New values
						// ...All checkbox set
						if(fxIsArray($_POST['cb0_'.$pid]) && fxIsArray($_POST['cb0_'.$pid][$mc]) && isset($_POST['cb0_'.$pid][$mc][$l]) && $_POST['cb0_'.$pid][$mc][$l])
							$nv_sa=$ov_pa;
						else
						{
							$nv_sa=0;
							// Bit: 1=Display (2), 2=Create (4), 3=Edit (8) + 4=Delete (16)
							for($c=1; $c<5; $c++)
							{
								if(($ov_pa & $bit[$c]) && fxIsArray($_POST['cb'.$c.'_'.$pid]) && fxIsArray($_POST['cb'.$c.'_'.$pid][$mc]) && isset($_POST['cb'.$c.'_'.$pid][$mc][$l]) && $_POST['cb'.$c.'_'.$pid][$mc][$l])
									$nv_sa += $bit[$c];
							}
						}
fxDebug(array('$ov_tr'=>$ov_tr, '$ov_pa'=>$ov_pa, '$ov_sa'=>$ov_sa, '$ov_id'=>$ov_id, '$ov_tn'=>$ov_tn, '$nv_sa'=>$nv_sa), $l.': $ov='.$ov, 3);

						if(!isset($sa[$pid]))
							$sa[$pid]=array();
						$sa[$pid][$ov_tr]=$nv_sa;

						// Setting changed?
						if(!$err && ($nv_sa != $ov_sa))
						{
							// Create or change entry
							if($nv_sa)
							{
								// ...Create
								if(!$ov_id)
								{
									$ov_id=id_max('rechte_trans','rechte_trans_id');

									$sql  = "INSERT INTO rechte_trans (mandanten_id, rechte_trans_id, trans_id, benutzergr_id, aktionscode)";
									$sql .= " VALUES (".$GLOBALS['fxpglobals']['client'].", ".$ov_id.", ".$ov_tr.", ".$id.", ".$nv_sa.")";
									$err=db_x($sql);
fxDebug($err,$sql, 2);
									if(!$err)
										$ins++;
								}
								// ...Change
								else
								{
									$sql="UPDATE rechte_trans SET aktionscode=".$nv_sa." WHERE rechte_trans_id=".$ov_id." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
									$err=db_x($sql);
fxDebug($err,$sql, 2);
									if(!$err)
										$upd++;
								}
							}
							// Delete entry
							else if($ov_id)
							{
								$sql="DELETE FROM rechte_trans WHERE rechte_trans_id=".$ov_id." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
								$err=db_x($sql);
fxDebug($err,$sql, 2);
								if(!$err)
									$del++;
							}

							if($err)
								error_msg(2186, false, $ov_tn);	// 2186: Beim Aktualisieren des Rechts für '$var1' ist ein Fehler aufgetreten.
						}
					}
				}
			}
		}
	}
fxDebug($sa,'$sa', 3);

	// Success messages
	if(!$err)
	{
		if($ins || $upd || $del)
		{
			if($ins)
			{
				if($ins == 1)
					error_msg(117);				// 117: Es wurde ein Datensatz erfolgreich angelegt.
				else
					error_msg(114,false,$ins);	// 114: Es wurden $var1 Datensätze erfolgreich angelegt.
			}
			if($upd)
			{
				if($upd == 1)
					error_msg(118);				// 118: Es wurde ein Datensatz erfolgreich geändert.
				else
					error_msg(115,false,$upd);	// 115: Es wurden $var1 Datensätze erfolgreich geändert.
			}
			if($del)
			{
				if($del == 1)
					error_msg(119);				// 119: Es wurde ein Datensatz erfolgreich gelöscht.
				else
					error_msg(116,false,$del);	// 116: Es wurden $var1 Datensätze erfolgreich gelöscht.
			}
		}
		else
			error_msg(160);							// 160: Es wurden keine Änderungen durchgeführt.
	}
//$err=true;
}
?>