<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : pers_komrep_report_pre.inc                                   //
// Version     : 21.2                                                         //
// Begin       : 2021-01-15                                                   //
// Last Change : 2021-01-15                                                   //
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
 * PF 248: Communication Report
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

if(isset($index))
	unset($index);

// Personen
$psel_c='';

// Postleitzahl oder Länderkennung
$lkz=(int)$_POST['LandKZ'];
$pvon=trim($_POST['nv_text1']);
$pbis=trim($_POST['nv_text3']);
if($lkz || $pvon || $pbis)
{
	$sql="SELECT personen_id FROM adressen WHERE mandanten_id=".$GLOBALS['fxpglobals']['client'];
	if($lkz)
		$sql .= " AND landkz=".$lkz;
	if($pvon && $pbis)
		$sql .= " AND (plz>='".$pvon."' AND plz<='".$pbis."')";
	else if($pvon)
		$sql .= " AND plz>='".$pvon."'";
	else if($pbis)
		$sql .= " AND plz<='".$pbis."'";
	$sql .= " AND personen_id IN (".$pr_csv.")";
	$ad=db_values($sql, '*');
fxDebug($ad, $sql, 3);

	$psel_a=array();
	if(fxIsArray($ad))
	{
		foreach($ad as $a)
			$psel_a[$a['personen_id']]=true;
	}

	if(fxIsArray($psel_a))
		$psel_c=array_to_csv($psel_a);
	else
		$psel_c='0';
//fxDebug($psel_c, 'Personen eingeschränkt wegen Adresse');
}

// Personentyp
$ptyp='0';
if(fxIsArray($pt_arr))
{
	foreach($pt_arr as $pv)
	{
		$name='personentyp_'.$pv['wertetabid'];
		if($maskenfeldwerte[$name] || $_POST[$name] || ${$name})
			$ptyp .= ','.$pv['wertetabid'];
	}
}

// Kommunikationsdaten ermitteln
$sql  = "SELECT k.personen_id, p.personen_id AS ppersonen_id, k.kommunikation_txt, k.anmerkung, k.aenderungs_id, k.zeitstempel,";
$sql .= " p.pname, p.vorname, p.personentyp, p.geschlecht, p.elter";
$sql .= " FROM kommunikation k, personen p";
$sql .= " WHERE k.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
if(strlen($psel_c))
	$sql .= " AND k.personen_id IN (".$psel_c.")";
else
	$sql .= " AND k.personen_id IN (".$pr_csv.")";
if($_POST['KommunikationsTyp'])
	$sql .= " AND k.kommunikationstyp=".$_POST['KommunikationsTyp'];
$st=trim($_POST['Suchtext']);
if($st)
	$sql .= " AND (".db_like('k.kommunikation_txt', '%'.$st.'%')." OR ".db_like('k.anmerkung', '%'.$st.'%').")";
$sql .= " AND k.personen_id=p.personen_id AND p.personentyp IN (".$ptyp.")";
if(!$_POST['inaktiv'])
	$sql .= " AND (p.inaktiv IS NULL OR p.inaktiv=0)";
$sql .= " ORDER BY k.kommunikation_txt";
if($_POST['sortierreihe'] == 'ab')
	$sql .= " DESC";
$kd=db_values($sql, '*');
//echo(fxHtmlEncode($sql).'<hr />');
fxDebug($kd, '$kd gesamt', 2);

if(fxIsArray($kd))
{
	// Aussortieren
	$sok=0;
	foreach($kd as $knr => $kdat)
	{
		$sok++;
		$elt=(int)$kdat['elter'];
		$kd[$knr]['allelter']=array($elt => true);

		// [Keine]
		if($_POST['Elter'] == -1)
		{
			if($elt <= 0)
				continue;
		}
		else
		{
			// Alle Elter ermitteln
			while($elt > 0)
			{
				$sql="SELECT elter FROM personen WHERE personen_id=".$elt." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$elt=(int)db_value($sql);
//fxDebug($elt,$sql, 0);
				if(($elt <= 0) || $kd[$knr]['allelter'][$elt])
					break;

				$kd[$knr]['allelter'][$elt]=true;

				// Nur eine Ebene höher suchen
				break;
			}

			// [Alle Personen]
			if(!$_POST['Elter'] && (!$GLOBALS['fxpglobals']['person'] || ($GLOBALS['fxpglobals']['user'] == 1) || $GLOBALS['fxpglobals']['persdat']['roles'][-1] || $GLOBALS['fxpglobals']['persdat']['roles'][2]))
				continue;

			// Ausgewählte Person
			if(($kdat['personen_id'] == $_POST['Elter']) || ($kdat['ppersonen_id'] == $_POST['Elter']) || $kd[$knr]['allelter'][$_POST['Elter']])
				continue;

		}

		$sok--;
		unset($kd[$knr]);
	}
//fxDebug($kd,'$kd nach Rechtebearbeitung', 0);

	// Prüfen, ob max. Anzahl überschritten ist
	if($GLOBALS['fxpglobals']['settings']['maxliste'] && ($sok > $GLOBALS['fxpglobals']['settings']['maxliste']))
	{
		// 1743: Zu viele Listeneinträge $var1 gefunden! Entsprechend den aktuellen Einstellungen wird die Liste nach maximal $var2 Einträgen abgebrochen. Bitte schränken Sie die Auswahlkriterien ein oder kontaktieren Sie Ihren Administrator.
		error_msg(1743, false, $sok, $GLOBALS['fxpglobals']['settings']['maxliste']);
		$sok=$GLOBALS['fxpglobals']['settings']['maxliste'];
	}

	// Literal ändern
	$Kommunikation_txt_lit=meldung($_POST['KommunikationsTyp'], true);

	// Merkvariablen, ob Spalten ausgeblendet werden können
	$col_am=false;
	$col_nm=true; // Name wird immer eingeblendet, ausser die Spalte wurde deaktiviert
	$col_ad=false;
	$col_ap=false;
	$col_nr=false;
	$col_gv=false;

	// Merkvariable, welcher Datensatz bereits angezeigt wurde
	$shown=array();

	$pd=array();	// Personendaten
	$ap=array();	// Ansprechpartner
	$at=array();	// Adresstypen
	$lk=array();	// Länderkennzeichen
	$kt=array();	// Kommunikationstyp

	// Maskenvariablen
	$mc=$GLOBALS['_maskcounter'];
	$z=0;
	$fa=fxf_getMaskFields(494);

	$alnk='<img src="'.$GLOBALS['gfxpath'].'lh.png" width=12 height=10 align=center>';

	// Felder
	$fld=array('kommunikation_txt', 'anmerkung', 'pname', 'adressen_id', 'ansprechpartner', 'kommunikation_id', 'aenderungs_id', 'link');

	// Transaktionssprung abhängig vom Personentyp
	$trjump=array(FXP_PT_CLIENT=>166, FXP_PT_CLIENT_CP=>164, FXP_PT_CUSTOMER=>169, FXP_PT_CUSTOMER_CP=>170, FXP_PT_PARTNER=>173, FXP_PT_PARTNER_CP=>174, FXP_PT_EMPLOYEE=>167, FXP_PT_CONTRACTOR=>168, FXP_PT_CONTACT=>171, FXP_PT_CONTACT_CP=>172);

	// Personen-ID des Mandanten
	$sql="SELECT personen_id FROM personen WHERE personentyp=274 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$mpid=(int)db_value($sql);

	// Zähler
	$cnt=0;
	while(true)
	{
		// Elter ermitteln, der angezeigt werden soll
		$elt_akt=0;
		foreach($kd as $c => $kdat)
		{
			// Wurde ein Datensatz bereits angezeigt, dann nächster...
			if($shown[$c])
				continue;

			// Neuen aktuellen Elter anzeigen
			$elt_akt=(int)$kd[$c]['elter'];
			if(!$elt_akt)
				$elt_akt=$mpid;

			// evtl. Leerzeile
			if($z)
			{
				fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
				$z++;
			}
			$ear=get_pd($elt_akt);
//fxDebug($ear, '$ear');
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['kommunikation_txt']='&nbsp;'.$ear['pic'].'&nbsp;<font class="s4b">'.$ear['name'].'</font>';
			// ...Anzeigelink
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['kommunikation_txt'] .= '&nbsp;&nbsp;'.fieldLinkImage('dlink'.$z, 'icsh', 'loader.php?url=pers_info.inc&pid='.$elt_akt);
			// ...Änderungslink
			if($ptr[$ear['personentyp']])
			{
				$jtr=$trjump[$ear['personentyp']];
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['kommunikation_txt'] .= '&nbsp;'.fieldLinkImage('clink'.$z, 'icch', '?nid=1&aktion='.FXP_CHANGE.'&pid='.$elt_akt.'&ltr='.$GLOBALS['fxpglobals']['tr'], '', $jtr);
			}

			// Zuständige Person=Elter des Elter
			if($elt_akt && ($elt_akt != $mpid))
			{
				$sql="SELECT elter FROM personen WHERE personen_id=".$elt_akt." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$zp=(int)db_value($sql);
				if($zp)
				{
					$zar=get_pd($zp);
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['kommunikation_txt'] .= '<br />&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font class="s4b lightgrey">&rarr;</font>&nbsp;'.$zar['pic'].'&nbsp;<font class="s2 darkgrey">'.$zar['name'].'</font>';
				}
			}
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['link']='hl';
			$z++;

			break;
		}

		// Sicherheitsvariable
		$oldz=$z;

		// Liste durchgehen
		foreach($kd as $c => $k)
		{
			// Wurde ein Datensatz bereits angezeigt, dann nächster...
			if($shown[$c])
			{
//echo($c.': Datensatz wurde schon angezeigt - SKIP!<br />');
				continue;
			}

			// Aktueller Elter entspricht nicht dem gewollten, dann nächster...
			$elt=(int)$k['elter'];
			if(!$elt)
				$elt=$mpid;
			if($elt != $elt_akt)
			{
//echo($c.': Aktueller Elter ('.$elt.') entspricht nicht dem gewollten ('.$elt_akt.') - SKIP!<br />');
				continue;
			}

			$shown[$c]=true;

			// Nummer
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['kommunikation_txt']='~'.text_wrap($k['kommunikation_txt'], true, 56);

			// Anmerkung
			if($k['anmerkung'] && $_POST['anmerkung_anz'])
			{
				$anm=$k['anmerkung'];
				$stp=strpos($anm, '[Start]');
				if($stp !== false)
				{
					$anm='... '.substr($anm, $stp);
					$stp=strpos($anm, '[Ende]');
					if($stp !== false)
						$anm=substr($anm, 0, $stp+6).' ...';
				}
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['anmerkung']='~<font class="grey">'.text_wrap($anm, true, 56).'</font>';

				$col_am=true;
			}

			// Name
			$pid=$k['personen_id'];
			$ar=get_pd($pid, $k);
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pname']='~'.$ar['pic'].'&nbsp;'.$ar['name'];
			// ...Anzeigelink
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pname'] .= '&nbsp;&nbsp;'.fieldLinkImage('dplink'.$z, 'icsh', 'loader.php?url=pers_info.inc&pid='.$pid);
			// ...Änderungslink
			if($ptr[$k['personentyp']])
			{
				$jtr=$trjump[$k['personentyp']];
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pname'] .= '&nbsp;'.fieldLinkImage('cplink'.$z, 'icch', '?nid=1&aktion=3&pid='.$pid.'&ltr='.$GLOBALS['fxpglobals']['tr'], '', $jtr);
			}

			// Adresse
			if(fxIsArray($ar['adr']))
			{
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['adressen_id']='~'.$ar['adr'][0]['txt'];

				$col_ad=true;
			}

			// geänd. von
			$cpid=$k['aenderungs_id'];
			if($cpid)
			{
				$cpd=get_pd($cpid);
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['aenderungs_id']='~'.$cpd['pic'].'&nbsp;'.$cpd['name'].'<br />&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<i class="s2 lightgrey">('.get_entry($k['zeitstempel'], 'datetime').')</i>';

				$col_gv=true;
			}

			// Ansprechpartner
			if(($k['personentyp'] == FXP_PT_CLIENT_CP) || ($k['personentyp'] == FXP_PT_CUSTOMER_CP) || ($k['personentyp'] == FXP_PT_PARTNER_CP) || ($k['personentyp'] == FXP_PT_CONTACT_CP))
			{
				$aar=get_ap($pid);
				if(fxIsArray($aar))
				{
					$acnt=0;
					foreach($aar as $apid => $dmy)
					{
						// Neue Zeile
						if($acnt)
							$z++;
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ansprechpartner']='~';
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['kommunikation_id']='~';
						$apd=get_pd($apid);
//fxDebug($apd,$apid, 0);
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ansprechpartner'] .= $apd['pic'].'&nbsp;'.$apd['name'];
						// ...Anzeigelink
						$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ansprechpartner'] .= '&nbsp;&nbsp;'.fieldLinkImage('dalink'.$z, 'icsh', 'loader.php?url=pers_info.inc&pid='.$apid);
						// ...Änderungslink
						if($ptr[$apd['personentyp']])
						{
							$jtr=$trjump[$apd['personentyp']];
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ansprechpartner'] .= '&nbsp;'.fieldLinkImage('calink'.$z, 'icch', '?nid=1&aktion='.FXP_CHANGE.'&pid='.$apid.'&ltr='.$GLOBALS['fxpglobals']['tr'], '', $jtr);
						}

						// Nummer
						if(fxIsArray($apd['kom']))
						{
							foreach($apd['kom'] as $kcnt => $k)
							{
								if($kcnt)
									$GLOBALS['fxptdata']['lvalues'][$mc][$z]['kommunikation_id'] .= '<br /><br />';
								$cinf='';
								$cpid=$k['aenderungs_id'];
								if($cpid)
								{
									$cpd=get_pd($cpid);
									$cinf='&nbsp;&nbsp;<i class="s2 lightgrey">('.get_entry($k['zeitstempel'], 'datetime').':&nbsp;'.$cpd['name'].')</i>';
								}
								$GLOBALS['fxptdata']['lvalues'][$mc][$z]['kommunikation_id'] .= '<u class="grey">'.$k['kommunikationstyp'].':</u>'.$cinf.'<br />'.text_wrap($k['kommunikation_txt'], true, 59);
								if($k['anmerkung'] && $_POST['anmerkung_anz'])
									$GLOBALS['fxptdata']['lvalues'][$mc][$z]['kommunikation_id'] .= '<br /><i class="grey">'.text_wrap($k['anmerkung'], true, 56).'</i>';
							}

							$col_nr=true;
						}

						$acnt++;
					}

					$col_ap=true;
				}
			}

			$z++;

			$cnt++;
			if($cnt >= $sok)
				break;
		}

		if(($cnt >= $sok) || ($z == $oldz))
			break;
	}

	if($z)
	{
		fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
		$z++;
	}

	$cs=7;
	// Spalten ausblenden...
	if(!$col_am || !$_POST['rc_'.$GLOBALS['_masknumber'].'_Anmerkung'])			// ...Anmerkung
	{
		$Anmerkung_nv=true;
		$cs--;
	}
	if(!$col_nm || !$_POST['rc_'.$GLOBALS['_masknumber'].'_Pname'])				// ...Name
	{
		$Pname_nv=true;
		$cs--;
	}
	if(!$col_ad || !$_POST['rc_'.$GLOBALS['_masknumber'].'_Adressen_ID'])		// Adresse
	{
		$Adressen_ID_nv=true;
		$cs--;
	}
	if(!$col_ap || !$_POST['rc_'.$GLOBALS['_masknumber'].'_Ansprechpartner'])	// ...Ansprechpartner
	{
		$Ansprechpartner_nv=true;
		$cs--;
	}
	if(!$col_nr || !$_POST['rc_'.$GLOBALS['_masknumber'].'_Kommunikation_ID'])	// ...Nummer
	{
		$Kommunikation_ID_nv=true;
		$cs--;
	}
	if(!$col_gv || !$_POST['rc_'.$GLOBALS['_masknumber'].'_Aenderungs_ID'])		// ...Geänd. von
	{
		$Aenderungs_ID_nv=true;
		$cs--;
	}

	if($col_gv && $_POST['rc_'.$GLOBALS['_masknumber'].'_Aenderungs_ID'])
		$ls='aenderungs_id';
	else if($col_nr && $_POST['rc_'.$GLOBALS['_masknumber'].'_Kommunikation_ID'])
		$ls='kommunikation_id';
	else if($col_ap && $_POST['rc_'.$GLOBALS['_masknumber'].'_Ansprechpartner'])
		$ls='ansprechpartner';
	else if($col_ad && $_POST['rc_'.$GLOBALS['_masknumber'].'_Adressen_ID'])
		$ls='adressen_id';
	else if($col_nm && $_POST['rc_'.$GLOBALS['_masknumber'].'_Pname'])
		$ls='pname';
	else if($col_am && $_POST['rc_'.$GLOBALS['_masknumber'].'_Anmerkung'])
		$ls='anmerkung';
	else
		$ls='kommunikation_txt';

	// Überschriften setzen
	if(fxIsArray($GLOBALS['fxptdata']['lvalues']) && fxIsArray($GLOBALS['fxptdata']['lvalues'][$mc]))
	{
		foreach($GLOBALS['fxptdata']['lvalues'][$mc] as $z => $lw)
		{
			if($lw['link'] != 'hl')
				continue;
			fxf_listSectHeader($mc, $z, $lw['kommunikation_txt'], 'kommunikation_txt', $ls, $cs, 0);
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['link']='~|';
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * ???FUNCTION???
 *
 * @param various $pid - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $ta  - Optional parameter (default=false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_pd($pid, $ta=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pid=(int)$pid;

	// Personendaten bereits vorhanden, dann sofort zurückliefern...
	if(isset($GLOBALS['pd'][$pid]))
		return $GLOBALS['pd'][$pid];

	// ...sonst aus Datenbank ermitteln, falls Datenarray nicht mitgegeben wurde
	if(!fxIsArray($ta))
	{
		$sql="SELECT pname, vorname, personentyp, geschlecht, elter FROM personen WHERE personen_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$arr=db_values($sql);

		// Wurden keine Personendaten gefunden, dann Benutzerdaten ermitteln
		if(!fxIsArray($arr))
		{
			$sql="SELECT FROM benutzer WHERE benutzer_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$pbid=db_value($sql);
			if($pbid)
			{
				$sql="SELECT personen_id, pname, vorname, personentyp, geschlecht, elter FROM personen WHERE personen_id=".$pbid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$arr=db_values($sql);
			}
		}
	}
	else
		$arr=array('pname'=>$ta['pname'], 'vorname'=>$ta['vorname'], 'personentyp'=>$ta['personentyp'], 'geschlecht'=>$ta['geschlecht'], 'elter'=>$ta['elter']);

	if(fxIsArray($arr))
	{
		$nm=$arr['pname'];
		$arr['vorname']=trim($arr['vorname']);
		if($arr['vorname'])
			$nm .= ', '.$arr['vorname'];

		$arr['name']=$nm;
		$arr['pic']=person_pic($pid, 20, $arr['geschlecht'], $GLOBALS['fxpglobals']['client'], false);
	}
	else
	{
		$arr['name']='???';
		$arr['pic']='<img src="'.$GLOBALS['gfxpath'].'pspbl3.png" border=0 height=18 hspace=0 vspace=0 align=middle>';
	}

	// Adressdaten ermitteln
	$sql="SELECT adresstyp, strasse, landkz, plz, ort, aenderungs_id, zeitstempel FROM adressen WHERE personen_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY zeitstempel";
	$adr=db_values($sql, '*');
	if(fxIsArray($adr))
	{
		$arr['adr']=$adr;
		foreach($arr['adr'] as $z => $a)
		{
			// Adresstyp
			if(!$GLOBALS['at'][$a['adresstyp']])
				$GLOBALS['at'][$a['adresstyp']]=meldung($a['adresstyp'], true);

			// Länderkennzeichen
			if(!$GLOBALS['lk'][$a['landkz']])
				$GLOBALS['lk'][$a['landkz']]=meldung($a['landkz'], true);

			$arr['adr'][$z]['adresstyp']=$GLOBALS['at'][$a['adresstyp']];
			$arr['adr'][$z]['landkz']=$GLOBALS['lk'][$a['landkz']];

			$arr['adr'][$z]['txt']=$arr['adr'][$z]['strasse'].'<br /><b>'.$arr['adr'][$z]['landkz'].'-'.$arr['adr'][$z]['plz'].'</b> '.$arr['adr'][$z]['ort'];
		}
	}
	else
		$arr['adr']=false;


	// Kommunikationsdaten ermitteln
	$sql="SELECT kommunikationstyp, kommunikation_txt, anmerkung, aenderungs_id, zeitstempel FROM kommunikation WHERE personen_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY kommunikationstyp, zeitstempel";
	$kom=db_values($sql, '*');
	if(fxIsArray($kom))
	{
		$arr['kom']=$kom;
		foreach($arr['kom'] as $z => $k)
		{
			// Adresstyp
			if(!$GLOBALS['kt'][$k['kommunikationstyp']])
				$GLOBALS['kt'][$k['kommunikationstyp']]=meldung($k['kommunikationstyp'], true);

			$arr['kom'][$z]['kommunikationstyp']=$GLOBALS['kt'][$k['kommunikationstyp']];
		}
	}
	else
		$arr['kom']=false;

	$GLOBALS['pd'][$pid]=$arr;
	return $GLOBALS['pd'][$pid];
}

/**
 * ???FUNCTION???
 *
 * @param various $pid - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_ap($pid)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pid=(int)$pid;

	// Personen-IDs bereits vorhanden, dann sofort zurückliefern...
	if(isset($GLOBALS['ap'][$pid]))
		return $GLOBALS['ap'][$pid];

	// ...sonst aus Datenbank ermitteln
	$sql="SELECT personen_id FROM personen WHERE elter=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$tmp=db_values($sql, '*');
	if(fxIsArray($tmp))
	{
		foreach($tmp as $t)
			$GLOBALS['ap'][$pid][$t['personen_id']]=true;
	}
	else
		$GLOBALS['ap'][$pid]=false;

	return $GLOBALS['ap'][$pid];
}
?>