<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : zk_aend_list_db.inc                                          //
// Version     : 21.2                                                         //
// Begin       : 2020-11-12                                                   //
// Last Change : 2020-11-12                                                   //
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
 * PF 146: Time Transfers
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$chg=0;
$zdbg=false;

// Alle Einträge durchgehen
if(fxIsArray($_POST['Zeit_ID'][$mc]))
{
//fxDebug('_POST');

	$old_pinfo=array();
	$new_pinfo=array();
	$phr=array();
	$inv=array();

	$gvon='00000000000000';	// Gültigkeitsbereich Leistungsartenzuordnung
	$gbis='99999999999999';

	foreach($_POST['Zeit_ID'][$mc] as $lz => $zid)
	{
		if($zdbg || ($GLOBALS['__DGBLVL'] == 4))
		{
			if($lz)
				echo('<br><hr>');
			echo('Line <b>'.$lz.'</b>:<br><br>');
		}

		$val_zeilennummer=$lz;

		$opid=(int)$_POST['ID'][$mc][$lz];
		$olid=(int)$_POST['kostenstelle_id'][$mc][$lz];

		$upos=strpos($_POST['Projekt_IDopen'][$mc][$lz],'_');
		if($upos !== false)
		{
			$npid=(int)substr($_POST['Projekt_IDopen'][$mc][$lz], 0, $upos);
			$nlid=(int)substr($_POST['Projekt_IDopen'][$mc][$lz], $upos+1);
		}
		else
		{
			$npid=(int)$_POST['Projekt_IDopen'][$mc][$lz];
			$nlid=0;
		}
		if($npid < 0)
		{
			$npid=$opid;
			$nlid=$olid;
		}

		$onv=0;
		// Hidden Checkbox übermittelt "Ja", falls gesetzt, sonst leer
		if($_POST['nv_check1'][$mc][$lz])
			$onv=1;
		$nnv=0;
		if($_POST['nicht_abrechenbar'][$mc][$lz])
			$nnv=1;

		// Checkbox nicht angehakt, dann überspringen
		if(!$_POST['Checkbox'][$mc][$lz])
		{
			if($zdbg || ($GLOBALS['__DGBLVL'] == 4))
				echo('<font class="red">Checkbox not ticked... SKIP</font><br>');
			continue;
		}

		if($zdbg || ($GLOBALS['__DGBLVL'] == 4))
			echo('$npid='.$npid.', $opid='.$opid.' -- $nlid='.$nlid.', $olid='.$olid.' -- $nnv='.$nnv.', $onv='.$onv.'<br>');

		// Keine Änderung, dann überspringen
		if(($npid == $opid) && ($nlid == $olid) && ($nnv == $onv))
		{
			if($zdbg || ($GLOBALS['__DGBLVL'] == 4))
				echo('<font class="blue">No changes... SKIP</font><br>');
			continue;
		}

		// Prüfen, ob Leistungsart angegeben werden muss
		if($kr_aktiv && $kr_lamuss && ($npid > 0) && !$nlid)
		{
			error_msg(805, array('Projekt_IDopen')); // 805: Es <B>muss</B> eine Leistungsart statt einer Aufgabe ausgewählt werden!
			$err=true;
			continue;
		}

		// Änderungen an der Aufgabe
		if(!$err && (($npid != $opid) || ($nnv != $onv)))
		{
			$cc='';

			// Standard-EK und -VK der Mitarbeiter ermitteln
			$persid=(int)$_POST['nv_text3'][$mc][$lz];
			if(!fxIsArray($aarray) && !fxIsArray($aarray[$persid]) && !$aarray[$persid]['artikel_id'])
				continue;

			if(!strlen($aarray[$persid]['ek_standard']))
			{
				$sql="SELECT ek, vk FROM artikel WHERE personen_id=".$persid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND (sachmittel_id IS NULL OR sachmittel_id=0) AND (archiv IS NULL OR archiv=0)";
				$std_arr=db_values($sql);
//fxDebug($std_arr, $sql);
				if(!fxIsArray($std_arr))
					$std_arr=array('ek'=>0.00, 'vk'=>0.00);

				$aarray[$persid]['ek_standard']=$std_arr['ek'];
				$aarray[$persid]['vk_standard']=$std_arr['vk'];
			}

			$ek_standard=$aarray[$persid]['ek_standard'];
			$vk_standard=$aarray[$persid]['vk_standard'];

			// Mandantenzugehörigkeit des Projekts
			if($opid && !$old_pinfo[$opid])
			{
				$sql="SELECT projektart, vorgangsnummer FROM projekte WHERE projekt_id=".$opid." AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")";
				$old_pinfo[$opid]=db_values($sql);
			}

			// Mandantenzugehörigkeit des Projekts
			if($npid && !$new_pinfo[$npid])
			{
				$sql  = "SELECT p.mandanten_id, p.mutterprojekt_id, p.vorgangsnummer, p.name_projekt, p.kunde, b.abrechnungsart, b.rechnung_std_ext_off";
				$sql .= " FROM projekte p, budget_summe b";
				$sql .= " WHERE p.projekt_id=".$npid." AND b.projekt_id=p.projekt_id AND p.mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].") AND b.mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$new_pinfo[$npid]=db_values($sql);
			}

			// Kostensätze des Mitarbeiters für die jeweilige Aufgabe oder Standardkostensätze, falls nicht zugeordnet
			$phr_id=$persid.'_'.$npid;
			if($npid && !isset($phr[$phr_id]))
			{
				$sql  = "SELECT a.ek_projekt, a.vk_projekt";
				$sql .= " FROM artikelzuordnung a, maplanung m";
				$sql .= " WHERE m.personen_id=".$persid." AND m.projekt_id=".$npid." AND m.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND a.maplanung_id=m.maplanung_id AND a.mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$costs=db_values($sql);
//fxDebug($costs, $sql);
				if(fxIsArray($costs))
					$phr[$phr_id]=$costs;
			}
			if(!isset($phr[$phr_id]))
			{
				if($zdbg || ($GLOBALS['__DGBLVL'] == 4))
					echo('Employee has no costs assigned for this task - using <b>standard costs</b>...<br>');
				$phr[$phr_id]=array('ek_projekt'=>$ek_standard, 'vk_projekt'=>$vk_standard);
			}

			// EK/VK
			if($zdbg || ($GLOBALS['__DGBLVL'] == 4))
				echo('<b>Person '.$persid.'</b> PP/SP</b>: standard='.$ek_standard.'/'.$vk_standard.' -- task='.$phr[$phr_id]['ek_projekt'].'/'.$phr[$phr_id]['vk_projekt'].'<br>');

			$EK=get_double(round($phr[$phr_id]['ek_projekt'] * ($_POST['Zeitdaten'][$mc][$lz] / 3600), $GLOBALS['fxpglobals']['settings']['nkstellen']));
			$VK=get_double(round($phr[$phr_id]['vk_projekt'] * ($_POST['Zeitdaten'][$mc][$lz] / 3600), $GLOBALS['fxpglobals']['settings']['nkstellen']));
			$MVK=0.00;
			if($zdbg || ($GLOBALS['__DGBLVL'] == 4))
				echo('<b>Hours:</b> '.($_POST['Zeitdaten'][$mc][$lz] / 3600).' -&gt; PP='.$EK.', SP='.$VK.'<br>');

			// Aufgabe geändert?
			if($npid != $opid)
			{
				if(strlen($cc))
					$cc .= ", ";

				if($npid)
					$cc .= "projekt_id=".$npid.", mutterprojekt_id=".$new_pinfo[$npid]['mutterprojekt_id'].", zkunde=".$new_pinfo[$npid]['kunde'].", kosten_ek=".$EK.", kosten_vk=".$VK.", kosten_mvk=".$MVK;
				else
					$cc .= "projekt_id=0, mutterprojekt_id=0, kosten_ek=0, kosten_vk=0, kosten_mvk=0";
			}

			// Änderungen an "verrechenbar"
			if($nnv != $onv)
			{
				if(strlen($cc))
					$cc .= ", ";
				if(!$nnv)
					$nnv="NULL";
				$cc .= "nicht_abrechenbar=".$nnv;
			}

			// Rechnung verändern?
			if($npid && !isset($inv[$npid]))
			{
				$inv[$npid]=array('abrechnungsart'=>0, 'rechnungs_id'=>"NULL", 'positions_id'=>"NULL");
				$sql="SELECT abrechnungsart FROM budget_summe WHERE projekt_id=".$npid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$aa=(int)db_value($sql);
//fxDebug($aa,$sql);
				if($aa)
					$inv[$npid]['abrechnungsart']=$aa;

				// 50 = P-FP = Fetspreis (Personen)
				$sql="SELECT rechnungs_id, positions_id FROM rechnungsposition WHERE projekt_id=".$npid." AND ist_sachmittel=50 AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY rechnungs_id";
				$fpid=db_values($sql);
//fxDebug($fpid,$sql);
				if($fpid['rechnungs_id'])
				{
					$inv[$npid]['rechnungs_id']=(int)$fpid['rechnungs_id'];
					$inv[$npid]['positions_id']=(int)$fpid['positions_id'];
				}
			}
			if(strlen($cc))
				$cc .= ", ";
			if($npid && isset($inv[$npid]))
				$cc .= "rechnungs_id=".$inv[$npid]['rechnungs_id'].", positions_id=".$inv[$npid]['positions_id'];
			else
				$cc .= "rechnungs_id=NULL, positions_id=NULL";

			// Zuordnung in Zeitdaten ändern
			$sql="UPDATE zeitdaten SET ".$cc." WHERE zeit_id=".$zid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$err=db_x($sql);
			if($zdbg || ($GLOBALS['__DGBLVL'] == 4))
				fxDebug($sql, '$sql: $err='.$err);

			// Zuordnung in Tätigkeiten ändern
			if(!$err && ($npid != $opid))
			{
				$sql="UPDATE taetigkeiten SET projekt_id=".$npid." WHERE zeit_id=".$zid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$err=db_x($sql);
				if($zdbg || ($GLOBALS['__DGBLVL'] == 4))
					fxDebug($sql, '$sql: $err='.$err);
			}

			// Aufwand und verbrauchtes Budget von der alten Aufgabe subtrahieren (außer von Unbekannt)
			if(!$err && ($npid != $opid) && $opid)
			{
				if($zdbg || ($GLOBALS['__DGBLVL'] == 4))
					echo('&middot; <b>Subtract effort and used budget from old task</b>...<br>');
				if($_POST['Zeitdaten'][$mc][$lz])
				{
					$err=Zeiterfassung($opid, -$_POST['Zeitdaten'][$mc][$lz]);
					if($zdbg || ($GLOBALS['__DGBLVL'] == 4))
						echo('&middot; <b>$err</b>=Zeiterfassung('.$opid.', '.(-$_POST['Zeitdaten'][$mc][$lz]).'); -&gt; '.$err.'<br>');
				}
				// Budgets von Teilprojekten evtl. abziehen
				if(!$err && fxIsArray($old_pinfo[$opid]) && ($old_pinfo[$opid]['projektart'] < FXP_PRJ_TASK) && (($EK > 0.00) || ($VK > 0.00)))
				{
					if(!isset($old_pinfo[$opid]['pids']))
					{
						$nvn=$old_pinfo[$opid]['vorgangsnummer'];
						$nvs='';
						while(strlen($nvn))
						{
							if(strlen($nvs))
								$nvs .= " OR ";
							$nvs .= "vorgangsnummer='".$nvn."'";
							$nvn=substr($nvn,0,-5);
						}
//echo('$nvs='.$nvs.'<br>');
						if(strlen($nvs))
						{
							$sql="SELECT projekt_id FROM projekte WHERE (".$nvs.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
							$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
							if(fxIsArray($tmp))
							{
								$old_pinfo[$opid]['pids']='';
								foreach($tmp as $t)
								{
									if(strlen($old_pinfo[$opid]['pids']))
										$old_pinfo[$opid]['pids'] .= ',';
									$old_pinfo[$opid]['pids'] .= $t['projekt_id'];
								}
							}
						}
//fxDebug($old_pinfo[$opid], '$old_pinfo['.$opid.']');
					}

					if(isset($old_pinfo[$opid]['pids']))
					{
						if($zdbg || ($GLOBALS['__DGBLVL'] == 4))
							echo('&middot; <b>Subtract used budget from old project</b>...<br>');
						$sql  = "UPDATE budget_summe SET budget_pers_int_verbr=(budget_pers_int_verbr - ".$EK."), budget_pers_ext_verbr=(budget_pers_ext_verbr - ".$VK.")";
						$sql .= " WHERE projekt_id IN (".$old_pinfo[$opid]['pids'].") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
						$err=db_x($sql);
						if($zdbg || ($GLOBALS['__DGBLVL'] == 4))
							fxDebug($sql, '$sql: $err='.$err);
					}
				}

				// Ist-Zeiten (Start- und Endzeiten) evtl. anpassen
				if(!$err)
				{
					$err=ist_zeiten($opid);
					if($zdbg || ($GLOBALS['__DGBLVL'] == 4))
						echo('&middot; <b>$err</b>=ist_zeiten('.$opid.'); -&gt; '.$err.'<br>');
				}
			}

			// Aufwand und verbrauchtes Budget auf die neue Aufgabe aufaddieren
			if(!$err && ($npid != $opid) && $npid)
			{
				if($zdbg || ($GLOBALS['__DGBLVL'] == 4))
					echo('&middot; <b>Add effort and used budget to new task</b>...<br>');
				if($_POST['Zeitdaten'][$mc][$lz])
				{
					$err=Zeiterfassung($npid, $_POST['Zeitdaten'][$mc][$lz]);
					if($zdbg || ($GLOBALS['__DGBLVL'] == 4))
						echo('&middot; <b>$err</b>=Zeiterfassung('.$npid.', '.$_POST['Zeitdaten'][$mc][$lz].'); -&gt; '.$err.'<br>');
					if($err)
						error_msg($err);
				}
				// Ist-Zeiten (Start- und Endzeiten) evtl. anpassen
				if(!$err)
				{
					$err=ist_zeiten($npid);
					if($zdbg || ($GLOBALS['__DGBLVL'] == 4))
						echo('&middot; <b>$err</b>=ist_zeiten('.$npid.'); -&gt; '.$err.'<br>');
				}
			}

			// Abrechenbare Beträge neu kalkulieren, falls nur "nicht verrechenbar" verändert wurde bei Stundenbasisaufgaben (außer von Unbekannt)
			if(!$err && ($npid == $opid) && $npid && fxIsArray($new_pinfo[$npid]) && ($new_pinfo[$npid]['abrechnungsart'] == FXP_INV_HOUR) && ($VK > 0.00))
			{
				if($zdbg || ($GLOBALS['__DGBLVL'] == 4))
					echo('&middot; <b>Recalculate invoiceable amounts if "not invoiceable" alone was changed for hourly based tasks</b>...<br>'.$nl);

				$vn=$new_pinfo[$npid]['vorgangsnummer'];
				$vns="vorgangsnummer='".$vn."'";
				while(strlen($vn) > 4)
				{
					$vn=substr($vn, 0, -5);
					$vns .= " OR vorgangsnummer='".$vn."'";
				}

				$ropp="-";
				if($onv)
					$ropp="+";

				// Offenen Rechnungsbetrag anpassen
				$sql  = "UPDATE budget_summe SET rechnung_std_ext_off=(rechnung_std_ext_off ".$ropp." ".$VK.")";
				$sql .= " WHERE projekt_id IN (SELECT projekt_id FROM projekte WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND (".$vns.")) AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$err=db_x($sql);
				if($zdbg || ($GLOBALS['__DGBLVL'] == 4))
					fxDebug($sql, '$sql: $err='.$err);
			}
		}

		// Änderungen an der Leistungsart
		if(!$err && ($nlid != $olid))
		{
			// Alte Zuordnung löschen
			if($olid && !$nlid)
				$sql="DELETE FROM kostenzuordnung WHERE kostenstelle_id=".$olid." AND id=".$zid." AND kostenstellenart=".FXP_TE_TIMEREC." AND kostendefinition=903 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			// Neue Zuordnung erstellen
			else if(!$olid && $nlid)
			{
				$sql  = "INSERT INTO kostenzuordnung";
				$sql .= " (mandanten_id, kostenstelle_id, id, kostenstellenart, kostendefinition, gueltig_von, gueltig_bis)";
				$sql .= " VALUES";
				$sql .= " (".$GLOBALS['fxpglobals']['client'].", ".$nlid.", ".$zid.", ".FXP_TE_TIMEREC.", 903, '".$gvon."', '".$gbis."')";
			}
			// Zuordnung ändern
			else
			{
				$sql  = "UPDATE kostenzuordnung SET";
				$sql .= " kostenstelle_id=".$nlid.", gueltig_von='".$gvon."', gueltig_bis='".$gbis."'";
				$sql .= " WHERE id=".$zid." AND kostenstellenart=".FXP_TE_TIMEREC." AND kostendefinition=903 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			}
			$err=db_x($sql);
			if($zdbg || ($GLOBALS['__DGBLVL'] == 4))
				fxDebug($sql, '$sql: $err='.$err);
		}

		if($err)
		{
			$em='';
			if($err !== true)
				$em=':<br /><br /><font class=red>&quot;'.meldung($err,true).'&quot;</font>';
			error_msg(150, false, $new_vals['name_projekt'].' [<b>'.$lz.'</b>]'.$em);	// 150: Fehler beim Ändern von $var1!
			break;
		}
		$chg++;
	}
}

if(!$err)
{
	if($chg == 1)
		error_msg(118, false);			// 118: Es wurde ein Datensatz erfolgreich geändert.
	else if($chg > 1)
		error_msg(115, false, $chg);	// 115: Es wurden $var1 Datensätze erfolgreich geändert.
	else
		error_msg(160, false);			// 160: Es wurden keine Änderungen durchgeführt.

	if($zdbg || ($GLOBALS['__DGBLVL'] == 4))
		$err=true;
	else if($chg)
		$maske340_nv=true;
}
?>