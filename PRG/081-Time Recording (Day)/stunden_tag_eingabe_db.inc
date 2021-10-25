<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : stunden_tag_eingabe_db.inc                                   //
// Version     : 21.2                                                         //
// Begin       : 2020-11-09                                                   //
// Last Change : 2020-11-09                                                   //
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
 * PF 81: Time Recording (Day)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

if(!$err && $db_update && ($Button_OK || ($Dummy == FXP_DELETE)))
{
	$mid=$GLOBALS['fxpglobals']['client'];
	$budgettype=strtoupper(substr($GLOBALS['fxpglobals']['settings']['budgetart'],0,1));
	$uea=array();
	$prj_completed=$fertig;
	$prj_addeffort=0;

	$ins=0;
	$upd=0;
	$del=0;

	$db_fertig='NULL';
	if($fertig)
		$db_fertig=1;

	$db_fg_relativ='NULL';
	if(strlen($fg_relativ))
		$db_fg_relativ=$fg_relativ;

	$zn=array();
	$zo=array();

	$zaa=array();
	if(!$sum)
	{
		$sdatum=substr($cdatum,0,8);
		$zaa[$sdatum]=array('zaw'=>$zaw, 'zap'=>$zap);
	}
	else
	{
		$cdatum=$startdatum;
		$sd=substr($startdatum,0,8);
		$sdatum=$sd;
		if(strlen($enddatum))
			$ed=substr($enddatum,0,8);
		else
			$ed=$sd;
		if(($ed == $sd) || ($Dummy == FXP_DELETE))
			$zaa[$sd]=array('zaw'=>$zaw, 'zap'=>"NULL");
		else
		{
			// Alle Tage ermitteln
			$da=weekdays($sd,$ed);
//fxDebug($da,'Alle Tage: $da');

			// ...Freie Tage abziehen
			$sh=0.00;
			if(fxIsArray($da))
			{
				$sql="SELECT wochenarbeitstd,arbeitstage,montag,dienstag,mittwoch,donnerstag,freitag,samstag,sonntag FROM madaten WHERE personen_id=".$vb_pid." AND mandanten_id=".$mid;
				$mad=db_values($sql);
				if(fxIsArray($mad))
				{
//fxDebug($mad,$sql);
					if($mad['arbeitstage'])
						$sh=round($mad['wochenarbeitstd']/$mad['arbeitstage'],2);
					if($sh > 0.00)
					{
						$wds=(int)$mad['sonntag'].(int)$mad['montag'].(int)$mad['dienstag'].(int)$mad['mittwoch'].(int)$mad['donnerstag'].(int)$mad['freitag'].(int)$mad['samstag'];
//fxDebug($wds,'$wds');
						$dn=array();
						foreach($da as $d => $wd)
						{
							if((int)substr($wds,$wd,1) > 0)
								$dn[$d]=$sh;
						}
						$da=$dn;
					}
					else
						$da=array();
				}
//fxDebug($da,'abzgl. freier Tage: $da');
			}

			// ...Feiertage abziehen
			if(fxIsArray($da))
			{
				$hda=get_feiertage($sd,$ed,$mid);
//fxDebug($hda,'$hda');
				if(fxIsArray($hda))
				{
					$dn=array();
					foreach($da as $d => $s)
					{
						$n=0.00;
						if(!isset($hda[$d]))
							$n=$s;
						else if($hda[$d]['typ'] < 1.00)
							$n=$s-($sh*$hda[$d]['typ']);
						if($n > 0.00)
							$dn[$d]=$n;
					}
					$da=$dn;
				}
//fxDebug($da,'abzgl. Feiertage: $da');
			}

			// ...Abwesenheitstage abziehen
			if(fxIsArray($da))
			{
				$sda=get_sondertage($sd,$ed,$vb_pid,0,false,false);
//fxDebug($sda,'$sda');
				if(fxIsArray($sda))
				{
					$dn=array();
					foreach($da as $d => $s)
					{
						$n=0.00;
						if(!isset($sda[$d]))
							$n=$s;
						else if($sda[$d][-1]['stunden_termin'] > 0.00)
							$n=$s-$sda[$d][-1]['stunden_termin'];
						if($n > 0.00)
							$dn[$d]=$n;
					}
					$da=$dn;
				}
//fxDebug($da,'abzgl. Abwesenheitstage: $da');
			}
//fxDebug($da,'FINAL: $da');

			if(fxIsArray($da))
			{
				$t=0.00;
				foreach($da as $d => $s)
					$t += $s;
//fxDebug($t,'$t');
				$ld='';
				$rt=0.00;
				foreach($da as $d => $s)
				{
					$ld=$d;
					$zaa[$d]=array('zaw'=>round(($zaw/$t)*$s,0), 'zap'=>"NULL");
					$rt += $zaa[$d]['zaw'];
				}
//fxDebug($rt,'$rt: '.$ld);

				if($rt != $zaw)
					$zaa[$ld]['zaw'] += ($zaw-$rt);
			}

			if(!sizeof($zaa))
				$zaa[$sd]=array('zaw'=>$zaw, 'zap'=>"NULL");
		}
	}
//fxDebug($zaa,'$zaa: '.sizeof($zaa));

	$ozi=0;
	$zta=array();
	if($sum && ($Dummy == FXP_CHANGE))
	{
		$sg_zid=(int)db_value("SELECT zeiteinheit FROM zeitdaten WHERE zeit_id=".$Zeit_ID." AND mandanten_id=".$mid);
		if($sg_zid == $Zeit_ID)	// Master
		{
			// Falls bereits ein abhängiger Zeitsummensatz abgerechnet oder freigegeben wurde, dann nur aktuellen Satz ändern
			$acc=true;
			$sql="SELECT z.zeit_id,z.projekt_id,z.ist_beginn,z.zeitaufwand,z.freigabe,z.rechnungs_id,t.taetigkeit_id FROM zeitdaten z, taetigkeiten t WHERE z.zeiteinheit=".$sg_zid." AND z.mandanten_id=".$mid." AND t.zeit_id=z.zeit_id AND t.mandanten_id=".$mid." ORDER BY z.ist_beginn";
			$zta=db_values($sql,'*');
//fxDebug($zta,$sql);
			if(fxIsArray($zta))
			{
				foreach($zta as $t)
				{
					if(($t['rechnungs_id']) || ($t['freigabe'] && ($t['freigabe'] > ($GLOBALS['fxpglobals']['persdat']['role']+1))))
					{
						$acc=false;
						break;
					}
				}
			}
			if(!$acc)
				$zta=array();
		}
	}
//fxDebug($zta,'$zta');

	$szaw=$zaw;
	$szaw_alt=0;
	$astuff=false;

	$sg_zid=0;
	$ztc=0;
	foreach($zaa as $cdatum => $ca)
	{
		$sdatum=min($sdatum, substr($cdatum,0,8));

		$zaw=$ca['zaw'];
		$zap=$ca['zap'];

		switch($Dummy)
		{
			case FXP_CREATE:
				$Zeit_ID=id_max('zeitdaten','zeit_id');
				$Taetigkeit_ID=id_max('taetigkeiten','taetigkeit_id');

				// Ist_Verzögerung wird auf bisherige Ist-Verzögerung aufaddiert (aktionsabhängig)
				if(!$sum)
					$prj_addeffort=$Ist_Verzoeg_up;

			case FXP_CHANGE:
				if(fxIsArray($zta))
				{
					if(isset($zta[$ztc]))
					{
						if(!$ozi)
							$ozi=$Zeit_ID;
						$Zeit_ID=$zta[$ztc]['zeit_id'];
						$Taetigkeit_ID=$zta[$ztc]['taetigkeit_id'];
					}
					else
					{
						$Zeit_ID=id_max('zeitdaten','zeit_id');
						$Taetigkeit_ID=id_max('taetigkeiten','taetigkeit_id');
					}
				}
				$ztc++;

				$zea=db_values("SELECT zeit_id,projekt_id,zeitaufwand FROM zeitdaten WHERE zeit_id=".$Zeit_ID." AND mandanten_id=".$mid);
				$zex=0;
				if(fxIsArray($zea))
					$zex=$zea['zeit_id'];
				$tex=(int)db_value("SELECT taetigkeit_id FROM taetigkeiten WHERE taetigkeit_id=".$Taetigkeit_ID." AND mandanten_id=".$mid);
//fxDebug($zex,'$zex');
//fxDebug($tex,'$tex');

				if($Projekt_ID > 0)
				{
					if(!isset($uea[$Projekt_ID]))
						$uea[$Projekt_ID]=array('old'=>0, 'new'=>0);
					$uea[$Projekt_ID]['new'] += $zaw;
				}

				if($zex && ($zea['projekt_id'] > 0))
				{
					if(!isset($uea[$zea['projekt_id']]))
						$uea[$zea['projekt_id']]=array('old'=>0, 'new'=>0);
					$uea[$zea['projekt_id']]['old'] += $zea['zeitaufwand'];
				}

				if(!$sg_zid && $sum)
				{
					if($Dummy == FXP_CREATE)
						$sg_zid=$Zeit_ID;
					else
						$sg_zid=(int)db_value("SELECT zeiteinheit FROM zeitdaten WHERE zeit_id=".$Zeit_ID." AND mandanten_id=".$mid);
				}

				// Ist_Verzögerung wird auf bisherige ist Verzögerung aufaddiert (aktionsabhängig)
				if(!$sum && $zex)
				{
					$verz_alt=(int)db_value("SELECT ist_verzoeg FROM zeitdaten WHERE zeit_id=".$Zeit_ID." AND mandanten_id=".$mid);
					$prj_addeffort=$Ist_Verzoeg_up-$verz_alt;
				}

				if($this_pid > 0)
					$pd=db_values("SELECT mutterprojekt_id,mandanten_id FROM projekte WHERE projekt_id=".$this_pid." AND mandanten_id=".$mid);
				else
					$pd=array('mutterprojekt_id'=>0, 'mandanten_id'=>$mid);

				// Freigabe + nicht_abrechenbar
				$adata=db_values("SELECT freigabe,nicht_abrechenbar FROM zeitdaten WHERE zeit_id=".$Zeit_ID." AND mandanten_id=".$mid);
				$Freigabe=$adata['freigabe'];
				if(!$Freigabe)
					$Freigabe=0;

				// Kunde
				if(!$Kunde)
					$Kunde=0;

				// Zeiteinheit + Zeitart
				if($sum)
				{
					$tunit=$sg_zid;
					$ttype=FXP_TE_TIMEREC_SUM;
				}
				else
				{
					$tunit='NULL';
					$ttype=FXP_TE_TIMEREC;
				}

				// EK + VK ermitteln
				$kek=0.00;
				$kvk=0.00;
				$kmvk=0.00;
				if($zaw || ($budgettype != 'N'))
				{
					$ekvk_sql="SELECT ek,vk,artikel_id FROM artikel WHERE personen_id=".$vb_pid." AND (sachmittel_id=0 OR sachmittel_id IS NULL) AND (archiv=0 OR archiv IS NULL) AND mandanten_id=".$mid;
					$Kostendaten=db_values($ekvk_sql);
					if(fxIsArray($Kostendaten))
					{
						$artikel_id=$Kostendaten['artikel_id'];
						$kek=get_double($Kostendaten['ek']);
						// 12.07.2011: Anweisung WB - Bei Zeiterfassung auf "unbekannt" kein VK
						if($this_pid > 0)
						{
							$kvk=get_double($Kostendaten['vk']);

							$kst_sql="SELECT ek_projekt,vk_projekt FROM artikelzuordnung WHERE artikel_id=".$artikel_id." AND projekt_id=".$this_pid." AND mandanten_id=".$mid;
							$kosten=db_values($kst_sql);
							if(fxIsArray($kosten))
							{
								// Mandantenübergreifend
								if($mid != $pd['mandanten_id'])
									$kmvk=get_double($kosten['vk_projekt']);
								else
								{
									$kek=get_double($kosten['ek_projekt']);
									$kvk=get_double($kosten['vk_projekt']);
								}
							}
						}
					}

					if(!$kek)
						$kek=0.00;
					if(!$kvk)
						$kvk=0.00;
					if(!$kmvk)
						$kmvk=0.00;

					$kek=round((($zaw/3600) * $kek), $GLOBALS['fxpglobals']['settings']['nkstellen']);
					$kvk=round((($zaw/3600) * $kvk), $GLOBALS['fxpglobals']['settings']['nkstellen']);
					$kmvk=round((($zaw/3600) * $kmvk), $GLOBALS['fxpglobals']['settings']['nkstellen']);

					if($budgettype == 'E')	// Nur externes Budget
						$kek=0.00;
					if($budgettype == 'I')	// Nur internes Budget
					{
						$kvk=0.00;
						$kmvk=0.00;
					}
				}

				$nabr='NULL';
				if($nicht_abrechenbar)
					$nabr=1;

				if($sum)
				{
					$ibz=0;
					$iez=0;
				}
				else
				{
					$ibz=$Ist_Beg_Zeit;
					$iez=$Ist_End_Zeit;
					if(($ibz === '') || ($iez === ''))
					{
						$ibz=0;
						$iez=0;
					}
				}

				$rech_id=0;
				$pos_id=0;
				if($Projekt_ID < 0) // Gleitzeit
				{
					$kek=0;
					$kvk=0;
					$kmvk=0;
					$nabr='NULL';
				}
				else if(!$Projekt_ID)
					$Projekt_ID=0;
				else
				{
					// Bei Festpreis-Projekten prüfen, ob bereits abgerechnet: 50 = P-FP = Fetspreis (Personen)
					$fpsql="SELECT rechnungs_id, positions_id FROM rechnungsposition WHERE projekt_id=".$this_pid." AND ist_sachmittel=50 AND mandanten_id=".$mid." ORDER BY rechnungs_id";
					$fpval=db_values($fpsql);
//fxDebug($fpval,$fpsql);
					if($fpval['rechnungs_id'])
						$rech_id=$fpval['rechnungs_id'];
					if($fpval['positions_id'])
						$pos_id=$fpval['positions_id'];
				}

				if($dl_offline)
					$Offline=(int)$dl_offline;
				else
					$Offline='NULL';

				$zd = array(
					'mandanten_id'			=> $mid,
					'zuordnungs_mid'		=> $pd['mandanten_id'],
					'personen_id'			=> $vb_pid,
					'zeit_id'				=> $Zeit_ID,
					'projekt_id'			=> $Projekt_ID,
					'mutterprojekt_id'		=> $pd['mutterprojekt_id'],
					'rechnungs_id'			=> $rech_id,
					'positions_id'			=> $pos_id,
					'zkunde'				=> $Kunde,

					'soll_beginn'			=> 'NULL',
					'soll_ende'				=> 'NULL',
					'ist_beginn'			=> "'".substr($cdatum,0,8).substr(validate_entry($ibz,$verr,'zeit'), -6)."'",
					'ist_ende'				=> "'".substr($cdatum,0,8).substr(validate_entry($iez,$verr,'zeit'), -6)."'",
					'kw'					=> kw($cdatum),
					'pause'					=> $zap,
					'zeitaufwand'			=> $zaw,

					'kosten_ek'				=> $kek,
					'kosten_vk'				=> $kvk,
					'kosten_mvk'			=> $kmvk,
					'mwst_satz'				=> get_double($GLOBALS['fxpglobals']['settings']['mwst_satz']),

					'rk_ek'					=> 'NULL',
					'rk_vk'					=> 'NULL',
					'rk_ort1'				=> 'NULL',
					'rk_ort2'				=> 'NULL',
					'rk_km'					=> 'NULL',
					'rk_anznacht'			=> 'NULL',
					'rk_kategorie'			=> 'NULL',

					'nicht_abrechenbar'		=> $nabr,
					'rk_nicht_abrechenbar'	=> 'NULL',
					'soll_verzoeg'			=> 'NULL',
					'ist_verzoeg'			=> $Ist_Verzoeg_up,
					'zeiteinheit'			=> $tunit,
					'verz_grund'			=> 'NULL',

					'zeitart'				=> $ttype,
					'zeitgruppe'			=> 'NULL',
					'freigabe'				=> $Freigabe,
					'fertig'				=> $db_fertig,
					'fg_relativ'			=> $db_fg_relativ,
					'offline_zd'			=> $Offline
				);
fxDebug($zd, 'ZEITDATEN-Array', 3);

				$zk='';
				$zv='';
				if(!$zex)	// Zeitdaten neuanlegen
				{
					$ins++;
					foreach($zd as $ky => $vl)
					{
						$zk .= ", ".$ky;
						$zv .= ", ".$vl;
					}
					$query_zeit="INSERT INTO zeitdaten (".substr($zk,2).") VALUES (".substr($zv,2).")";
				}
				else					// Zeitdaten ändern
				{
					$upd++;
					$szaw_alt += db_value("SELECT zeitaufwand FROM zeitdaten WHERE zeit_id=".$Zeit_ID." AND mandanten_id=".$mid);
					foreach($zd as $ky => $vl)
						$zk .= ", ".$ky."=".$vl;
					$query_zeit="UPDATE zeitdaten SET ".substr($zk,2)." WHERE zeit_id=".$Zeit_ID." AND mandanten_id=".$mid;
				}
fxDebug($query_zeit, 'ZEIT:', 2);

				// Tätigkeiten
				$Taetigkeit_Text=trim($_POST['Taetigkeit_Text']);
				if(strlen($Taetigkeit_Text))
					$tt="'".convert_string($Taetigkeit_Text,'todb')."'";
				else
					$tt='NULL';
				$Anmerkung=trim($_POST['Anmerkung']);
				if(strlen($Anmerkung))
					$at="'".convert_string($Anmerkung,'todb')."'";
				else
					$at='NULL';
				$td = array(
					'mandanten_id'		=> $mid,
					'taetigkeit_id'		=> $Taetigkeit_ID,
					'personen_id'		=> $vb_pid,
					'projekt_id'		=> $Projekt_ID,
					'zeit_id'			=> $Zeit_ID,
					'taetigkeit_text'	=> $tt,
					'anmerkungintern'	=> $at,
					'kategorie_id'		=> 'NULL',
					'archiv'			=> 0
				);
fxDebug($td, 'TAETIGKEITEN-Array', 3);

				$tk='';
				$tv='';
				if(!$tex)	// Tätigkeiten neuanlegen
				{
					foreach($td as $ky => $vl)
					{
						$tk .= ", ".$ky;
						$tv .= ", ".$vl;
					}
					$query_taetigkeit="INSERT INTO taetigkeiten (".substr($tk,2).") VALUES (".substr($tv,2).")";
				}
				else					// Tätigkeiten ändern
				{
					foreach($td as $ky => $vl)
						$tk .= ", ".$ky."=".$vl;
					$query_taetigkeit="UPDATE taetigkeiten SET ".substr($tk,2)." WHERE taetigkeit_id=".$Taetigkeit_ID." AND mandanten_id=".$mid;
				}
fxDebug($query_taetigkeit, 'TAETIGKEIT:', 2);

				// Leistungsart
				$old_lart_id=(int)db_value("SELECT kostenstelle_id FROM kostenzuordnung WHERE id=".$Zeit_ID." AND mandanten_id=".$mid." AND kostenstellenart=".FXP_TE_TIMEREC." AND kostendefinition=903");
				$query_lart='';
				if($lart_id)
				{
					$gvon='00000000000000';
					$gbis='99999999999999';
					if(!$old_lart_id)					// Zuordnung neu anlegen
					{
						$query_lart  = "INSERT INTO kostenzuordnung";
						$query_lart .= " (mandanten_id, kostenstelle_id, id, kostenstellenart, kostendefinition, gueltig_von, gueltig_bis)";
						$query_lart .= " VALUES";
						$query_lart .= " (".$mid.", ".$lart_id.", ".$Zeit_ID.", ".FXP_TE_TIMEREC.", 903, '".$gvon."', '".$gbis."')";
					}
					else if($old_lart_id != $lart_id)	// Zuordnung ändern
					{
						$query_lart  = "UPDATE kostenzuordnung SET";
						$query_lart .= " kostenstelle_id=".$lart_id.", gueltig_von='".$gvon."', gueltig_bis='".$gbis."'";
						$query_lart .= " WHERE id=".$Zeit_ID." AND mandanten_id=".$mid." AND kostenstellenart=".FXP_TE_TIMEREC." AND kostendefinition=903";
					}
fxDebug($query_lart, 'LEISTUNGSART:', 2);
				}
				else if($old_lart_id)					// Zuordnung löschen
				{
					$query_lart = "DELETE FROM kostenzuordnung WHERE kostenstelle_id=".$old_lart_id." AND id=".$Zeit_ID." AND mandanten_id=".$mid." AND kostenstellenart=".FXP_TE_TIMEREC." AND kostendefinition=903";
fxDebug($query_lart, 'LEISTUNGSART:', 2);
				}

				// Daten für E-Mail aufbereiten
				if(($Dummy == FXP_CHANGE) && !fxIsArray($zo))
				{
					$ms  = "SELECT z.projekt_id, z.ist_beginn, z.ist_ende, z.pause, z.zeitaufwand, t.taetigkeit_text";
					$ms .= " FROM zeitdaten z, taetigkeiten t WHERE t.zeit_id=z.zeit_id AND z.zeit_id=".$Zeit_ID." AND z.mandanten_id=".$mid." AND t.mandanten_id=".$mid;
					$tmp=db_values($ms);
//fxDebug($tmp,$ms);
					if(fxIsArray($tmp))
						$zo=$tmp;
				}

				// Daten in DB eintragen
				$err=db_x($query_zeit);
				if(!$err)
					$err=db_x($query_taetigkeit);
				if(!$err && $query_lart)
					$err=db_x($query_lart);

				// Evtl. Termine für Terminaufgaben anpassen
				if(!$err && sizeof($Old_Projekt) && ($Projekt_ID != $Old_Projekt['projekt_id']))
					$err=app_prj_timereg(-1, $Old_Projekt['projekt_id'], $vb_pid);
				if(!$err)
					$err=app_prj_timereg($Zeit_ID, $Projekt_ID, $vb_pid);
				if($err)
					error_msg(456+$Dummy, false);	// Fehler beim Anlegen/Ändern des Datensatzes

				if(!$err)
				{
					$astuff=true;
					// Daten für E-Mail aufbereiten
					if(!fxIsArray($zn))
					{
						$ms  = "SELECT z.projekt_id, z.ist_beginn, z.ist_ende, z.pause, z.zeitaufwand, t.taetigkeit_text";
						$ms .= " FROM zeitdaten z, taetigkeiten t WHERE t.zeit_id=z.zeit_id AND z.zeit_id=".$Zeit_ID." AND z.mandanten_id=".$mid." AND t.mandanten_id=".$mid;
						$tmp=db_values($ms);
//fxDebug($tmp,$ms);
						if(fxIsArray($tmp))
							$zn=$tmp;
					}
				}
			break;

			case FXP_DELETE:
				// Daten für E-Mail aufbereiten
				if(!fxIsArray($zo))
				{
					$ms  = "SELECT z.projekt_id, z.ist_beginn, z.ist_ende, z.pause, z.zeitaufwand, t.taetigkeit_text";
					$ms .= " FROM zeitdaten z, taetigkeiten t WHERE t.zeit_id=z.zeit_id AND z.zeit_id=".$Zeit_ID." AND z.mandanten_id=".$mid." AND t.mandanten_id=".$mid;
					$tmp=db_values($ms);
//fxDebug($tmp,$ms);
					if(fxIsArray($tmp))
						$zo=$tmp;
				}

				$sql="SELECT zeit_id,projekt_id,personen_id,zeitaufwand,freigabe,rechnungs_id,zeiteinheit,zeitart,ist_verzoeg FROM zeitdaten WHERE zeit_id=".$Zeit_ID." AND mandanten_id=".$mid;
				$zea=db_values($sql);
//fxDebug($zea,$sql);
				$zex=0;
				if(fxIsArray($zea))
					$zex=$zea['zeit_id'];
//fxDebug($zex,'$zex');

				if($zex)
				{
					$del++;
					$abrart=0;
					if($zea['projekt_id'] > 0)
					{
						if(!isset($uea[$zea['projekt_id']]))
							$uea[$zea['projekt_id']]=array('old'=>0, 'new'=>0);
						$uea[$zea['projekt_id']]['old'] += $zea['zeitaufwand'];

						$sql="SELECT abrechnungsart FROM budget_summe WHERE projekt_id=".$zea['projekt_id']." AND mandanten_id=".$mid;
						$abrart=(int)db_value($sql);
//fxDebug($abrart,$sql);
					}

					$zids=$Zeit_ID;
					$szid=0;

					if(($zea['zeitart'] == FXP_TE_TIMEREC_SUM) && ($zea['zeiteinheit'] == $Zeit_ID))
					{
						$sql="SELECT zeit_id,projekt_id,ist_beginn,zeitaufwand,freigabe,rechnungs_id FROM zeitdaten WHERE zeiteinheit=".$Zeit_ID." AND mandanten_id=".$mid." AND zeit_id<>".$Zeit_ID." ORDER BY ist_beginn";
						$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
						if(fxIsArray($tmp))
						{
							foreach($tmp as $t)
							{
								if(($t['rechnungs_id']) || ($t['freigabe'] && ($t['freigabe'] > ($GLOBALS['fxpglobals']['persdat']['role']+1))))
								{
									if(!$szid)
										$szid=$t['zeit_id'];
								}
								else
								{
									$del++;
									$zids .= ",".$t['zeit_id'];
									if($t['projekt_id'] > 0)
									{
										if(!isset($uea[$t['projekt_id']]))
											$uea[$t['projekt_id']]=array('old'=>0, 'new'=>0);
										$uea[$t['projekt_id']]['old'] += $t['zeitaufwand'];
									}
									$sdatum=min($sdatum, substr($t['ist_beginn'],0,8));
								}
							}
						}
					}

					if($zea['freigabe'] > ($GLOBALS['fxpglobals']['persdat']['role']+1))
					{
						$err=1614;
						error_msg($err,false);	// Der Datensatz darf nicht mehr gelöscht werden, da er bereits von einer höheren Instanz "freigegeben" wurde!
					}
					else if($zea['rechnungs_id'] && ($abrart == FXP_INV_HOUR))
					{
						$err=3004;
						error_msg($err,false);	// Der Datensatz darf nicht mehr gelöscht werden, da er bereits "abgerechnet" wurde!
					}
					else
					{
						// Ist_Verzögerung wird auf bisherige Ist Verzögerung aufaddiert (aktionsabhängig)
						if($zea['zeitart'] != FXP_TE_TIMEREC_SUM)
							$prj_addeffort=-$zea['ist_verzoeg'];

						// Datensätze löschen
						$err=deleteHourEntries($zids,$mid,$lart_id);

						// Evtl. bei untergeordneten Zeitsummensätzen neuen Master setzen
						if(!$err && $szid)
						{
							$sql="UPDATE zeitdaten SET zeiteinheit=".$szid." WHERE zeiteinheit=".$Zeit_ID." AND mandanten_id=".$mid;
							$err=db_x($sql);
fxDebug($sql, 'Bei untergeordneten Zeitsummensätzen neuen Master setzen', 2);
						}

						// Evtl. Termine für Terminaufgaben anpassen
						if(!$err)
							$err=app_prj_timereg(-1, $zea['projekt_id'], $zea['personen_id']);
						if($err)
							error_msg(460,false);	// Fehler beim Löschen des Datensatzes
					}
				}
			break;
		}
	}

	if($ozi)
	{
		$Zeit_ID=$ozi;
		// Übrige Summenzeitdatensätze löschen
		if(!$err && isset($zta[$ztc]))
		{
			$zea=db_values("SELECT zeit_id,projekt_id,personen_id,zeitaufwand,freigabe,zeiteinheit,zeitart FROM zeitdaten WHERE zeit_id=".$Zeit_ID." AND mandanten_id=".$mid);

			$zids='';
			while(isset($zta[$ztc]))
			{
//fxDebug($zta[$ztc],'Delete');
				$del++;
				if(strlen($zids))
					$zids .= ',';
				$zids .= $zta[$ztc]['zeit_id'];
				if($zta[$ztc]['projekt_id'] > 0)
				{
					if(!isset($uea[$zta[$ztc]['projekt_id']]))
						$uea[$zta[$ztc]['projekt_id']]=array('old'=>0, 'new'=>0);
					$uea[$zta[$ztc]['projekt_id']]['old'] += $zta[$ztc]['zeitaufwand'];
				}
				$sdatum=min($sdatum, substr($zta[$ztc]['ist_beginn'],0,8));
				$ztc++;
			}

			// Datensätze löschen
			if(strlen($zids))
				$err=deleteHourEntries($zids,$mid,$lart_id);

			// Evtl. Termine für Terminaufgaben anpassen
			if(!$err && ($zea['projekt_id'] > 0))
				$err=app_prj_timereg(-1, $zea['projekt_id'], $zea['personen_id']);
			if($err)
				error_msg(460,false);	// Fehler beim Löschen des Datensatzes
		}
	}

	// Alten Ist-Aufwand aktualisieren; Funktion passt ebenfalls das Budget an
	if(!$err && sizeof($uea))
	{
//fxDebug($uea,'$uea');
		foreach($uea as $prid => $pea)
		{
			if(!$err && ($pea['new'] != $pea['old']))
			{
				$err=Zeiterfassung($prid, $pea['new']-$pea['old'], 0, true);
				if($err)
					error_msg($err,false);
fxDebug($err.' = Zeiterfassung('.$prid.', '.($pea['new']-$pea['old']).');', '$err = Zeiterfassung(Projekt-ID, Zeitaufwandsdifferenz);', 2);
			}

			// Setzt Ist-Beginn der Aufgabe, falls noch nicht geschehen oder wenn dieser Wert früher ist. Selbiges wird bei den Eltern der Aufgabe durchgeführt
			if(!$err)
			{
				$err=ist_zeiten($prid);
				if($err)
					error_msg(1870,false);	// Fehler beim Aktualisieren der übergeordneten Start- und Endtermine!
fxDebug($err.' = ist_zeiten('.$prid.');', '$err = ist_zeiten(Projekt-ID);', 2);
			}
		}
	}

	// Projektfortschritt aktualisieren
	if(!$err && ($this_pid > 0) && strlen($fg_relativ))
	{
		$err=projektfortschritt($this_pid, $vb_pid, $fg_relativ, 0, 0, 0, 0, 'TR');
		if($err)
			error_msg($err,false);
fxDebug($err.' = projektfortschritt('.$this_pid.', '.$vb_pid.', '.$fg_relativ.');', '$err = projektfortschritt(Projekt-ID, Personen-ID, Fertigstellungsgrad);', 2);
	}

	// Bilanzen löschen, damit diese beim Report wieder aktualisiert wird
	if(!$err && $vb_pid && strlen($sdatum))
	{
		$err=bilanz_zeitkonto_loeschen($vb_pid, $sdatum);
		if($err)
			error_msg(1868,false);	// Fehler beim Löschen veralteter Bilanzdatensätze!
fxDebug($err.' = bilanz_zeitkonto_loeschen('.$vb_pid.', '.$sdatum.');', '$err = bilanz_zeitkonto_loeschen(Personen-ID, Startdatum);', 2);
	}

	$settings_backup=$GLOBALS['fxpglobals']['settings'];
	$lang_backup=$GLOBALS['fxpglobals']['lang'];

	$GLOBALS['_ua']=array();
	$GLOBALS['_ma']=array();

	$merr=false;

	// Editor
	$editor='';
	if($vb_pid != $GLOBALS['fxpglobals']['person'])
	{
		// 47: Edited by
		$editor='<table width="80%" cellpadding="8" cellspacing="0" style="border:1px solid #'.colorCode('red').';border-radius:4px;box-shadow:2px 2px 4px #'.colorCode('darkgrey').';"><tr><td>$var1: <b>'.getPersonName($GLOBALS['fxpglobals']['person'],false).'</b></td></tr></table>';
	}

	// E-Mail über Zeiterfassung an Person senden, ausser Ersteller entspricht der Person oder Senden deaktiviert
//fxDebug($zo,'$zo');
//fxDebug($zn,'$zn');
	if(!$err && ($vb_pid != $GLOBALS['fxpglobals']['person']) && (fxIsArray($zd) || fxIsArray($zo)) && check_mail(3033,'MA'))
	{
		// Get sender and recipient data
		$sender=$GLOBALS['fxpglobals']['person'];
		if(!isset($GLOBALS['_ua'][$sender]))
		{
			$pdata=get_email_adress($sender,true,true);
			if(fxIsArray($pdata) && fxIsArray($pdata[$sender]))
				$GLOBALS['_ua'][$sender]=$pdata[$sender];
			else
				$GLOBALS['_ua'][$sender]=false;
		}
		$recipient=$vb_pid;
		if(!isset($GLOBALS['_ua'][$recipient]))
		{
			$pdata=get_email_adress($recipient,true,true);
			if(fxIsArray($pdata) && fxIsArray($pdata[$recipient]))
				$GLOBALS['_ua'][$recipient]=$pdata[$recipient];
			else
				$GLOBALS['_ua'][$recipient]=false;
		}
		if(fxIsArray($GLOBALS['_ua'][$sender]) && fxIsArray($GLOBALS['_ua'][$recipient]))
		{
			$GLOBALS['fxpglobals']['settings']=$GLOBALS['_ua'][$recipient]['settings'];
			$GLOBALS['fxpglobals']['lang']=$GLOBALS['_ua'][$recipient]['settings']['sprache'];
			$lang=$GLOBALS['fxpglobals']['lang'];

			// External Time-Registration^Date|Time|Break|Task Name|Activity^The following time-registration entry has been created for you by {sen}^Your following time-registration entry has been {act} by {sen}^changed^deleted
			$zma=explode('^',get_message('mail_ext_treg'));
//fxDebug($zma,'$zma');

			// Title + headers
			$title=$zma[0];		// External Time-Registration
			$headers=$zma[1];	// Date|Time|Break|Task Name|Activity

			// Content
			$content='';
			if(strlen($editor))
				$content .= strtr($editor, array('$var1'=>meldung(47))).'<br /><br />';
			if($Dummy == FXP_CREATE)
			{
				// The following time-registration entry has been created for you by {sen}
				$content .= strtr($zma[2],array('{sen}'=>'<b>'.$GLOBALS['_ua'][$sender]['name2'].'</b>'));
			}
			else
			{
				// Your following time-registration entry has been {act} by {sen}
				if($Dummy == FXP_CHANGE)
					$zact=$zma[4];	// changed
				else
					$zact=$zma[5];	// deleted
				$content .= strtr($zma[3],array('{act}'=>$zact, '{sen}'=>'<b>'.$GLOBALS['_ua'][$sender]['name2'].'</b>'));
			}
			$content .= ':<br /><br />';

			// Data
			$cdata='N~';
			// ...Date
			if(fxIsArray($zo) && fxIsArray($zn) && (substr($zo['ist_beginn'],0,8) != substr($zn['ist_beginn'],0,8)))
				$cdata .= '<font style="color:#888888;">'.get_entry($zo['ist_beginn'],'datum').'</font>'.$GLOBALS['_carrow'].'<b>'.get_entry($zn['ist_beginn'],'datum').'</b>';
			else if(fxIsArray($zo))
				$cdata .= get_entry($zo['ist_beginn'],'datum');
			else
				$cdata .= get_entry($zn['ist_beginn'],'datum');
			// ...Time
			$cdzo='';
			if(fxIsArray($zo))
			{
				if($zo['ist_beginn'] != $zo['ist_ende'])
					$cdzo=get_entry($zo['ist_beginn'],'zeit').'-'.get_entry($zo['ist_ende'],'zeit').' '.meldung(3017);
				else
					$cdzo=get_entry($zo['zeitaufwand']/3600,'dezimal').' '.meldung(484);
			}
			$cdzn='';
			if(fxIsArray($zn))
			{
				if($zn['ist_beginn'] != $zn['ist_ende'])
					$cdzn=get_entry($zn['ist_beginn'],'zeit').'-'.get_entry($zn['ist_ende'],'zeit').' '.meldung(3017);
				else
					$cdzn=get_entry($zn['zeitaufwand']/3600,'dezimal').' '.meldung(484);
			}
			$cdata .= '|N~';
			if(strlen($cdzo) && strlen($cdzn) && ($cdzo != $cdzn))
				$cdata .= '<font style="color:#888888;">'.$cdzo.'</font>'.$GLOBALS['_carrow'].'<b>'.$cdzn.'</b>';
			else if(strlen($cdzo))
				$cdata .= $cdzo;
			else
				$cdata .= $cdzn;
			// ...Break
			$cdata .= '|N~';
			if(fxIsArray($zo) && fxIsArray($zn) && ($zo['pause'] != $zn['pause']))
				$cdata .= '<font style="color:#888888;">'.get_entry($zo['pause']/60,'ganzzahl').' '.meldung(856).'</font>'.$GLOBALS['_carrow'].'<b>'.get_entry($zn['pause']/60,'ganzzahl').' '.meldung(856).'</b>';
			else if(fxIsArray($zo))
				$cdata .= get_entry($zo['pause']/60,'ganzzahl').' '.meldung(856);
			else
				$cdata .= get_entry($zn['pause']/60,'ganzzahl').' '.meldung(856);
			// ...Task Name
			$cdata .= '|W40%~';
			if(fxIsArray($zo) && fxIsArray($zn) && ($zo['projekt_id'] != $zn['projekt_id']))
				$cdata .= '<font style="color:#888888;">'.getProjectName($zo['projekt_id']).'</font>'.$GLOBALS['_carrow'].'<b>'.getProjectName($zn['projekt_id']).'</b>';
			else if(fxIsArray($zo))
				$cdata .= getProjectName($zo['projekt_id']);
			else
				$cdata .= getProjectName($zn['projekt_id']);
			// ...Activity
			$cdata .= '|W60%~';
			if(fxIsArray($zo) && fxIsArray($zn) && ($zo['taetigkeit_text'] != $zn['taetigkeit_text']))
				$cdata .= '<font style="color:#888888;">'.$zo['taetigkeit_text'].'</font>'.$GLOBALS['_carrow'].'<b>'.$zn['taetigkeit_text'].'</b>';
			else if(fxIsArray($zo))
				$cdata .= $zo['taetigkeit_text'];
			else
				$cdata .= $zn['taetigkeit_text'];
			$content .= mailDataTable($headers, array(0=>$cdata));

			$GLOBALS['fxpglobals']['settings']=$settings_backup;
			$GLOBALS['fxpglobals']['lang']=$lang_backup;

			// Send email
			if(strlen($title) && strlen($content))
			{
				$send_error=send_mail($recipient,$title,$content);
				if($send_error)
				{
					$merr=true;
					error_msg(3031,false,'&quot;'.$title.'&quot;','<b>'.$GLOBALS['_ua'][$recipient]['name2'].'</b>');	// Error sending the email about $var1 to $var2!
				}
				else
					error_msg(3032,false,'<b>'.$GLOBALS['_ua'][$recipient]['name2'].'</b>','&quot;'.$title.'&quot;');	// $var1 was successfully informed by email about $var2.
			}
		}
	}

	$GLOBALS['fxpglobals']['settings']=$settings_backup;
	$GLOBALS['fxpglobals']['lang']=$lang_backup;

	// Zusatzaktionen, nur falls zu keinem Dummy-Projekt zugeordnet
	if(!$err && ($Projekt_ID > 0))
	{
		// Completion only if not already set as an appointment
		if(!$err && $prj_completed)
		{
			$sql="SELECT termin_id FROM termine WHERE projekt_id=".$Projekt_ID." AND terminart=1131 AND (erledigt=0 OR erledigt IS NULL) AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$tba=db_value($sql);
			if($tba)
				$prj_completed='';
		}

		// Completion and/or effort addition of a task
		if(!$err && ($Dummy != FXP_DELETE) && ($prj_completed || $prj_addeffort))
		{
			// Get all task data
			$pd=projektinfo($Projekt_ID, false, false, true);
//fxDebug($pd,'$pd');

			if(fxIsArray($pd))
			{
				$sent=0;
				$GLOBALS['_ea']=array();

				// Project leader
				$plid=getProjectleader($pd[$Projekt_ID]['data']['projektleiter'], $pd[$Projekt_ID]['data']['vertreter']);
//fxDebug($plid,'$plid');

				$pers_name=getPersonName($vb_pid,false);
				$task_name=no_null($pd[$Projekt_ID]['data']['vorgangsnummer']).' - '.$pd[$Projekt_ID]['data']['name_projekt'];

				// Insert addition task effort
				if(!$err && $prj_addeffort)
				{
					$sql  = "UPDATE zeitdaten";
					$sql .= " SET ist_verzoeg=(ist_verzoeg+(".$prj_addeffort.")) WHERE projekt_id IN (".superior_vn($Projekt_ID,true).") AND mandanten_id=".$mid." AND zeitart=102";
					$err=db_x($sql);
					if($err)
						error_msg(1871,false,'<b>'.$task_name.'</b>');	// Error inserting the additional effort for task $var1!!
				}

				// Create a completion project appointment
				if(!$err && $prj_completed && $plid)
				{
					// Get project leader data
					if(!isset($GLOBALS['_ua'][$plid]))
					{
						$pdata=get_email_adress($plid,true,true);
						if(fxIsArray($pdata) && fxIsArray($pdata[$plid]))
							$GLOBALS['_ua'][$plid]=$pdata[$plid];
						else
							$GLOBALS['_ua'][$plid]=false;
//fxDebug($GLOBALS['_ua'][$plid],'$GLOBALS['_ua']['.$plid.']');
					}

					if(fxIsArray($GLOBALS['_ua'][$plid]))
					{
						$GLOBALS['fxpglobals']['settings']=$GLOBALS['_ua'][$plid]['settings'];
						$GLOBALS['fxpglobals']['lang']=$GLOBALS['_ua'][$plid]['settings']['sprache'];
						$lang=$GLOBALS['fxpglobals']['lang'];

						// Task completion^$var1 reports the completion of task $var2^Actual^Plan^Ext.^Int.^AR~Actual/Plan|AC~Start Date|AC~End Date|AR~Days|AR~Effort|R3~&nbsp;|AR~Ext./Int.|AR~Plan. Budget|AR~Act. Budget|AR~Used Budget|AR~Marge|R3~&nbsp;|Description
						$md='email_prj_completion';
						if(!isset($GLOBALS['_ma'][$md.'_'.$lang]))
							$GLOBALS['_ma'][$md.'_'.$lang]=explode('^',get_message($md));

						$GLOBALS['fxpglobals']['settings']=$settings_backup;
						$GLOBALS['fxpglobals']['lang']=$lang_backup;

						$aid=id_max('termine','termin_id');
						$acont='NULL';
						if($vb_pid != $GLOBALS['fxpglobals']['person'])
							$acont=$GLOBALS['fxpglobals']['person'];
						$aname=$GLOBALS['_ma'][$md.'_'.$lang][0];															// Task completion
						$adesc=strtr($GLOBALS['_ma'][$md.'_'.$lang][1], array('$var1'=>$pers_name, '$var2'=>$task_name));	// $var1 reports the completion of task $var2

						$sql  = "INSERT INTO termine ";
						$sql .= "(";
						$sql .= " mandanten_id, personen_id, ansprechpartner, person_angelegt, name_termin, termin_id, mastertermin_id,";
						$sql .= " startzeit, endzeit, beschreibung, terminart, wiederholung, projekt_id, erledigt";
						$sql .= ") ";
						$sql .= "VALUES ";
						$sql .= "(";
						$sql .= " ".$GLOBALS['fxpglobals']['client'].", ".$plid.", ".$acont.", ".$vb_pid.", '".convert_string($aname,'todb')."', ".$aid.", ".$aid.",";
						$sql .= " '".$GLOBALS['datetime']."', '".$GLOBALS['datetime']."', '".convert_string($adesc,'todb')."', 1131, 0, ".$Projekt_ID.", 0";
						$sql .= ")";
						$err=db_x($sql);
//fxDebug($err,$sql);
						if($err)
							error_msg(1228,false,': <b>'.$aname.'</b>');	// 1228: Error creating the appointment$var1
					}
				}

				// Request additional effort and/or report the completion of a task
				$caa=array(1726=>$prj_addeffort, 1728=>$prj_completed);
				foreach($caa as $etype => $evalue)
				{
					if(!$err && $evalue)
					{
						$GLOBALS['fxpglobals']['settings']=$settings_backup;
						$GLOBALS['fxpglobals']['lang']=$lang_backup;
						$mpa=getProjectMailPersons(array(), $etype, $pd[$Projekt_ID]['data']);
//fxDebug($mpa,'$mpa: '.$etype);

						if(sizeof($mpa))
						{
							foreach($mpa as $pers => $dummy)
							{
								// Get recipient data
								if(!isset($GLOBALS['_ua'][$pers]))
								{
									$pdata=get_email_adress($pers,true,true);
									if(fxIsArray($pdata) && fxIsArray($pdata[$pers]))
										$GLOBALS['_ua'][$pers]=$pdata[$pers];
									else
										$GLOBALS['_ua'][$pers]=false;
//fxDebug($GLOBALS['_ua'][$pers],'$GLOBALS['_ua']['.$pers.']');
								}
								if(fxIsArray($GLOBALS['_ua'][$pers]) && strlen($GLOBALS['_ua'][$pers]['email']))
								{
									$email=$GLOBALS['_ua'][$pers]['email'];
									if(!isset($GLOBALS['_ea'][$pers]))
										$GLOBALS['_ea'][$pers]=array('email'=>$email);

									$GLOBALS['fxpglobals']['settings']=$GLOBALS['_ua'][$pers]['settings'];
									$GLOBALS['fxpglobals']['lang']=$GLOBALS['_ua'][$pers]['settings']['sprache'];
									$lang=$GLOBALS['fxpglobals']['lang'];

									if($etype == 1726)	// Additional effort^$var1 requests additional effort for task $var2^Actual^Plan^Ext.^Int.^AR~Actual/Plan|AC~Start Date|AC~End Date|AR~Days|AR~Effort|R3~&nbsp;|AR~Ext./Int.|AR~Plan. Budget|AR~Act. Budget|AR~Used Budget|AR~Marge|R3~&nbsp;|Description
										$md='email_prj_addeffort';
									else				// Completion^$var1 reports the completion of task $var2^Actual^Plan^Ext.^Int.^AR~Actual/Plan|AC~Start Date|AC~End Date|AR~Days|AR~Effort|R3~&nbsp;|AR~Ext./Int.|AR~Plan. Budget|AR~Act. Budget|AR~Used Budget|AR~Marge|R3~&nbsp;|Description
										$md='email_prj_completion';
									if(!isset($GLOBALS['_ma'][$md.'_'.$lang]))
										$GLOBALS['_ma'][$md.'_'.$lang]=explode('^',get_message($md));

									if(!isset($GLOBALS['_ea'][$pers][$md.$Projekt_ID]))
									{
										// ...Actual/Plan
										$data=array(
											0=>'N~AR~<b>'.$GLOBALS['_ma'][$md.'_'.$lang][2].'</b>',
											1=>'N~AR~<font style="color:#'.colorCode('darkgrey').';">'.$GLOBALS['_ma'][$md.'_'.$lang][3].'</font>'
										);
										// ...Start Date
										$data[0] .= '|AC~N~<b>'.get_entry($pd[$Projekt_ID]['data']['ist_beginn'],'datum').'</b>';
										$data[1] .= '|AC~N~<font style="color:#'.colorCode('darkgrey').';">'.get_entry($pd[$Projekt_ID]['data']['soll_beginn'],'datum').'</font>';
										// ...End Date
										if($etype == 1726)	// Additional effort
											$data[0] .= '|AC~N~&nbsp;';
										else
											$data[0] .= '|AC~N~<b style="color:#'.colorCode('blue').';">'.get_entry($GLOBALS['datetime'],'datum').'</b>';
										$data[1] .= '|AC~N~<font style="color:#'.colorCode('darkgrey').';">'.get_entry($pd[$Projekt_ID]['data']['soll_ende'],'datum').'</font>';
										// ...Days
										$adays=countWeekdays($pd[$Projekt_ID]['data']['ist_beginn'],$GLOBALS['datetime'], 1,1,1,1,1,1,1);
										$pdays=countWeekdays($pd[$Projekt_ID]['data']['soll_beginn'],$pd[$Projekt_ID]['data']['soll_ende'], 1,1,1,1,1,1,1);
										$data[0] .= '|AR~N~<b>'.get_entry($adays,'ganzzahl_tz').'</b>';
										$data[1] .= '|AR~N~<font style="color:#'.colorCode('darkgrey').';">'.get_entry($pdays,'ganzzahl_tz').'</font>';
										// ...Effort
										$data[0] .= '|AR~N~<b>'.get_entry(($pd[$Projekt_ID]['aufwand_ist']/3600),'dezimal').'</b> h';
										$data[1] .= '|AR~N~<font style="color:#'.colorCode('darkgrey').';">'.get_entry(($pd[$Projekt_ID]['aufwand_soll']/3600), 'dezimal').' h</font>';
										if($etype == 1726)	// Additional effort
										{
											$data[1] .= '&nbsp;<br /><b style="color:#'.colorCode('red').';">+ '.get_entry($evalue/3600,'dezimal').'</b> <font style="color:#'.colorCode('red').';">h</font>&nbsp;';
											$data[1] .= '<div style="border-top:1px solid #'.colorCode('lightgrey').';"></div><b>'.get_entry(($pd[$Projekt_ID]['aufwand_soll']+$evalue)/3600,'dezimal').'</b> h';
										}
										// ...(Empty)
										// ...Ext./Int.
										$data[0] .= '|N~AR~<b>'.$GLOBALS['_ma'][$md.'_'.$lang][4].'</b>';
										$data[1] .= '|N~AR~<font style="color:#'.colorCode('darkgrey').';">'.$GLOBALS['_ma'][$md.'_'.$lang][5].'</font>';
										// ...Plan. Budget
										$data[0] .= '|AR~N~<b>'.get_entry($pd[$Projekt_ID]['data']['budget_pers_ext_gepl']+$pd[$Projekt_ID]['data']['budget_mat_ext_gepl'],'dezimal').'</b> '.$GLOBALS['fxpglobals']['settings']['waehrung'];
										$data[1] .= '|AR~N~<font style="color:#'.colorCode('darkgrey').';">'.get_entry($pd[$Projekt_ID]['data']['budget_pers_int_gepl']+$pd[$Projekt_ID]['data']['budget_mat_int_gepl'],'dezimal').' '.$GLOBALS['fxpglobals']['settings']['waehrung'].'</font>';
										// ...Act. Budget
										$data[0] .= '|AR~N~<b>'.get_entry($pd[$Projekt_ID]['data']['budget_pers_ext_akt']+$pd[$Projekt_ID]['data']['budget_mat_ext_akt'],'dezimal').'</b> '.$GLOBALS['fxpglobals']['settings']['waehrung'];
										$data[1] .= '|AR~N~<font style="color:#'.colorCode('darkgrey').';">'.get_entry($pd[$Projekt_ID]['data']['budget_pers_int_akt']+$pd[$Projekt_ID]['data']['budget_mat_int_akt'],'dezimal').' '.$GLOBALS['fxpglobals']['settings']['waehrung'].'</font>';
										// ...Used Budget
										$data[0] .= '|AR~N~<b>'.get_entry($pd[$Projekt_ID]['data']['budget_pers_ext_verbr']+$pd[$Projekt_ID]['data']['budget_mat_ext_verbr'],'dezimal').'</b> '.$GLOBALS['fxpglobals']['settings']['waehrung'];
										$data[1] .= '|AR~N~<font style="color:#'.colorCode('darkgrey').';">'.get_entry($pd[$Projekt_ID]['data']['budget_pers_int_verbr']+$pd[$Projekt_ID]['data']['budget_mat_int_verbr'],'dezimal').' '.$GLOBALS['fxpglobals']['settings']['waehrung'].'</font>';
										// ...Marge
										$mcol='green';
										$Marge=$pd[$Projekt_ID]['data']['budget_pers_ext_verbr']+$pd[$Projekt_ID]['data']['budget_mat_ext_verbr']-$pd[$Projekt_ID]['data']['budget_pers_int_verbr']-$pd[$Projekt_ID]['data']['budget_mat_int_verbr'];
										if($Marge < 0.00)
											$mcol='red';
										$data[0] .= '|AR~R2~N~<b style="color:#'.colorCode($mcol).';">'.get_entry($Marge,'dezimal').'</b> <font style="color:#'.colorCode($mcol).';">'.$GLOBALS['fxpglobals']['settings']['waehrung'].'</font>';
										// ...(Empty)
										// ...Description
										$data[0] .= '|R2~'.$pd[$Projekt_ID]['data']['beschreibung'];

										$GLOBALS['_ea'][$pers][$md.$Projekt_ID]=array(
											'info'=>strtr($editor, array('$var1'=>meldung(47))),
											'title'=>$GLOBALS['_ma'][$md.'_'.$lang][0].': '.$task_name,
											'headline'=>strtr($GLOBALS['_ma'][$md.'_'.$lang][1], array('$var1'=>$pers_name, '$var2'=>$task_name)),
											'headers'=>$GLOBALS['_ma'][$md.'_'.$lang][6],
											'data'=>$data,
											'mbgcolor'=>colorCode('green')
										);
									}
								}
							}
						}
					}
				}

				// Create and send emails
				if(sizeof($GLOBALS['_ea']))
				{
//fxDebug($GLOBALS['_ea'],'$GLOBALS['_ea']');
					$sc=sendPersonMails($GLOBALS['_ea'],$GLOBALS['_ua']);
					if($sc === false)
					{
						$merr=true;
						$GLOBALS['fxpglobals']['settings']=$settings_backup;
						$GLOBALS['fxpglobals']['lang']=$lang_backup;
						error_msg(171);	// 171: Error sending the completion and/or additional effort email!
					}
					else
						$sent += $sc;
				}

				// Email success
				if(!$merr && $sent)
				{
					$GLOBALS['fxpglobals']['settings']=$settings_backup;
					$GLOBALS['fxpglobals']['lang']=$lang_backup;
					error_msg(173);	// 173: The emails regarding completion and/or additional effort were successfully sent.
				}
			}
		}
	}

	$GLOBALS['fxpglobals']['settings']=$settings_backup;
	$GLOBALS['fxpglobals']['lang']=$lang_backup;

	// Erfolgsmeldungen
//$err=true;
	if(!$err)
	{
		if(($Button_OK !== 'zeitdaten_import') && (($Dummy == FXP_CREATE) || ($Dummy == FXP_CHANGE)))
		{
			if(!$sum)
			{
				unset($Ist_Verzoeg);
				unset($Zeiteinheit_Ist_Verz);
				unset($Freigabe);
				unset($Ist_Beg_Zeit);
				unset($Pause);
			}

			unset($fertig);
			unset($nicht_abrechenbar);
			unset($fg_relativ);
			unset($Zeitaufwand);
			unset($Taetigkeit_Text);
			unset($Anmerkung);

			if($Dummy == FXP_CREATE)
			{
				if(!$sum && ($ztc < 4))
					$Ist_Beg_Zeit=$Ist_End_Zeit;
			}
			else
				$Dummy=FXP_CREATE;

			if(!$sum)
				unset($Ist_End_Zeit);
		}

		if($ins > 0)
		{
			if($ins == 1)
				error_msg(117,false);		// Es wurde ein Datensatz erfolgreich angelegt.
			else
				error_msg(114,false,$ins);	// Es wurden $var1 Datensätze erfolgreich angelegt.
		}
		if($upd > 0)
		{
			if($upd == 1)
				error_msg(118,false);		// Es wurde ein Datensatz erfolgreich geändert.
			else
				error_msg(115,false,$upd);	// Es wurden $var1 Datensätze erfolgreich geändert.
		}
		if($del > 0)
		{
			if($del == 1)
				error_msg(119,false);		// Es wurde ein Datensatz erfolgreich gelöscht.
			else
				error_msg(116,false,$del);	// Es wurden $var1 Datensätze erfolgreich gelöscht.
		}
		if(!$ins && !$upd && !$del)
			error_msg(160,false);			// Es wurden keine Änderungen durchgeführt.
	}
}
?>