<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : urlaub_vj_db.inc                                             //
// Version     : 21.1                                                         //
// Begin       : 2020-10-21                                                   //
// Last Change : 2021-04-07                                                   //
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
 * PF 142: Employee's Annual Vacation
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

if(!$err && $disp_list && $_POST['Button_Multi'])
{
	switch($GLOBALS['fxpglobals']['action'])
	{
		case FXP_CHANGE:
fxDebug($plist,'$plist', 3);
			if(fxIsArray($plist))
			{
				$chg=0;

				// Liste durchgehen
				foreach($plist as $pid => $p)
				{
					$ureset=false;
					foreach($p as $d)
					{
						$von=substr('0000'.$d['jahr'], -4).'0101000000';
						$bis=substr('0000'.$d['jahr'], -4).'1231240000';

						// Alten Wert updaten?
						if($d['artikel_id'])
						{
							$aid=(int)$d['artikel_id'];

							$sql  = "UPDATE artikel";
							$sql .= " SET soll_beginn='".$von."', soll_ende='".$bis."', urlaubsanspruch=".$d['ua'].", resturlaub=".$d['uu'].", archiv=0";
							$sql .= " WHERE artikel_id=".$aid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
						}
						// oder neuen Werte eintragen
						else
						{
							$aid=(int)id_max('artikel', 'artikel_id');

							$sql  = "INSERT INTO artikel";
							$sql .= " (mandanten_id, artikel_id, personen_id, sachmittel_id, ek, vk, mwst_satz, soll_beginn, soll_ende, urlaubsanspruch, resturlaub, archiv)";
							$sql .= " VALUES";
							$sql .= " (".$GLOBALS['fxpglobals']['client'].", ".$aid.", ".$pid.", -".FXP_AT_VACATION.", 0.00, 0.00, 0.00, '".$von."', '".$bis."', ".$d['ua'].", ".$d['uu'].", 0)";
						}
						$perr=db_x($sql);
fxDebug($perr,$sql, 2);
						if(!$perr && !$ureset)
						{
							$perr=reset_resturlaub($pid, $d['jahr']);
							$ureset=true;
						}
						if($perr)
						{
							$err=true;
							error_msg(479, false, '&quot;<b>'.$d['name'].'</b>&quot;');	// 479: Fehler bei der Neukalkulation der Resturlaubstage für $var1!
							break 2;
						}
						else
							$chg++;
					}
				}

				// Erfolgsmeldungen
				if(!$err)
				{
					if($chg == 1)
						error_msg(118, false);			// 118: Es wurde ein Datensatz erfolgreich geändert.
					else
						error_msg(115, false, $chg);	// 115: Es wurden $var1 Datensätze erfolgreich geändert.
				}
			}
			else
			{
				// 160: Es wurden keine Änderungen durchgeführt.
				error_msg(160, false);
			}
		break;

		case FXP_DELETE:
//fxDebug('_POST');
			// Aktuellen Schlüssel ermitteln
			reset($_POST['Personen_ID']);
			$mc=key($_POST['Personen_ID']);

			$del=0;
			if(fxIsArray($_POST['Personen_ID'][$mc]))
			{
				$ureset=0;
				foreach($_POST['Personen_ID'][$mc] as $ln => $pid)
				{
					$pid=(int)$pid;
					if(!$pid)
						continue;

					// Löschen?
					if($_POST['Jahr'][$mc][$ln])
					{
						$aid=(int)$_POST['Artikel_ID'][$mc][$ln];

						$sql="DELETE FROM artikel WHERE artikel_id=".$aid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
						$perr=db_x($sql);
fxDebug($perr,$sql, 2);
						if(!$perr && ($ureset != $pid))
						{
							$perr=reset_resturlaub($pid, (int)$_POST['nv_ganzzahl1'][$mc][$ln]);
							$ureset=$pid;
						}
						if($perr)
						{
							$err=true;
							error_msg(479, false, '&quot;<b>'.$_POST['Pname'][$mc][$ln].'</b>&quot;');	// 479: Fehler bei der Neukalkulation der Resturlaubstage für $var1!
							break;
						}
						else
							$del++;
					}
				}
			}

			// Erfolgsmeldungen
			if(!$err)
			{
				if(!$del)
					error_msg(1642, false);			// 1642: Es wurden keine Einträge angehakt.
				else if($del == 1)
					error_msg(119, false);			// 119: Es wurde ein Datensatz erfolgreich gelöscht.
				else
					error_msg(116, false, $del);	// 116: Es wurden $var1 Datensätze erfolgreich gelöscht.
			}
		break;
	}
//$err=true;
}
?>