<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : termine_einladung_pre.inc                                    //
// Version     : 21.1                                                         //
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
 * PF 27: Appointment Planner
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

$index=true;
$mc=$GLOBALS['_maskcounter'];

// Datum
$dat=$maskenfeldwerte['soll_beg_dtm'];
if(!strlen($dat))
	$dat=$GLOBALS['datetime'];

$datum=substr($dat, 0,8);
$sdatum=$datum;
//echo('<B>$datum:</B> '.$datum.'<BR>');
$ta=(int)substr($datum, 6,2);
$mo=(int)substr($datum, 4,2);
$ja=(int)substr($datum, 0,4);

// Startwochentag ermitteln (0=So, 1=Mo, ..., 6=Sa)
$wtg=fx_date('w', fx_mktime(0, 0, 0, $mo, $ta, $ja));
$stg=((int)$GLOBALS['fxpglobals']['settings']['ersterwt'])%7;
//echo('<B>$stg:</B> '.$stg.', <B>$wtg:</B> '.$wtg.'<BR>');
if($wtg != $stg)
	$von=fx_date('Ymd', fx_mktime(0, 0, 0, $mo, $ta+$stg-$wtg, $ja));
else
	$von=$datum;
$bis=fx_date('Ymd', fx_mktime(0, 0, 0, $mo, $ta+$stg+20-$wtg, $ja));
//echo('-> <B>$von:</B> '.$von.', <B>$bis:</B> '.$bis.'<BR>');

// Alle Personen finden, die zum angezeigten Termin bereits eingeladen sind
$inv=array();
if($MasterTermin_ID && ($GLOBALS['fxpglobals']['action'] != FXP_CREATE))
{
	$sql="SELECT termin_id FROM termine WHERE mastertermin_id=".$MasterTermin_ID." AND terminart=".FXP_AT_INVITATION." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$ete=db_value($sql);
	if($ete)	// Einladungstermin
	{
		$sql  = "SELECT personen_id, termin_id, bestaetigung_anmerk AS anmerkung";
		$sql .= " FROM termine";
		$sql .= " WHERE mastertermin_id=".$MasterTermin_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$tmp=db_values($sql,'*');
		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
				$inv[$t['personen_id']]=$t;
		}
	}
}
//fxDebug($inv,'$inv');

$ics='';
if(fxIsArray($inv))
{
	foreach($inv as $pid => $ia)
	{
		if(strlen($ics))
			$ics .= ',';
		$ics .= $pid;
	}
}

// Alle möglichen Personen (ausser Admin) finden
$prs=array();
if($Termin_ID == $MasterTermin_ID)
{
	// Alle Personen (entspr. Filter) finden
	$GLOBALS['ignore_inaktiv']		= true;
	$GLOBALS['ignore_personentyp']	= true;
	// Beim Sysadmin, Admins, Int. und Ext. Mitarbeiter die Personenrechteprüfung unberücksichtigt lassen
	if(!$GLOBALS['fxpglobals']['person'] || ($GLOBALS['fxpglobals']['user'] == 1) || ($GLOBALS['fxpglobals']['persdat']['personentyp'] == 275) || ($GLOBALS['fxpglobals']['persdat']['personentyp'] == 840))
		$GLOBALS['ignore_personenrechte']=true;

	$sql  = "SELECT p.personen_id, p.personentyp, p.pname, p.vorname, p.geschlecht, 0 AS termin_id, NULL AS anmerkung, b.e_mail_adresse";
	$sql .= " FROM personen p LEFT JOIN benutzer b ON (b.personen_id=p.personen_id AND b.mandanten_id=p.mandanten_id)";
//	$sql .= " WHERE (p.inaktiv=0 OR p.inaktiv IS NULL) AND (p.personentyp=275 OR p.personentyp=840)";
	$sql .= " WHERE (";
	if(($GLOBALS['fxpglobals']['action'] == FXP_CREATE) || ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE))
	{
		$sql .= "(";
		$sql .= "p.personen_id=".$GLOBALS['fxpglobals']['person'];
		if($persid)
			$sql .= " OR p.personen_id=".$persid;
		$sql .= " OR ((p.inaktiv=0 OR p.inaktiv IS NULL) AND (p.personentyp=".FXP_PT_CLIENT_CP." OR p.personentyp=".FXP_PT_EMPLOYEE." OR p.personentyp=".FXP_PT_CONTRACTOR." OR p.personentyp=".FXP_PT_CUSTOMER_CP." OR p.personentyp=".FXP_PT_PARTNER_CP." OR p.personentyp=".FXP_PT_CONTACT_CP."))";
		$sql .= ")";
		$sql .= filter_person('and', false, false, 'personenliste');
	}
	if(($GLOBALS['fxpglobals']['action'] == FXP_CREATE) || ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE))
		$sql .= " OR ";
	if(strlen($ics))
		$sql .= "p.personen_id IN (".$ics.")";
	else
		$sql .= "p.personen_id=0";
	$sql .= ") AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND b.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$sql .= " ".db_orderby('p.pname', 'p.vorname');
	$prs =db_values($sql, '*');

	$maske112_flt=$GLOBALS['personenliste_flt'];
//fxDebug($prs, $sql);
}

// Liste aller Personen erstellen
$lst=array(
	0					=> false,	// Selbst
	34					=> false,	// Zugeordnet
	FXP_PT_EMPLOYEE		=> false,
	FXP_PT_CONTRACTOR	=> false,
	FXP_PT_CUSTOMER_CP	=> false,
	FXP_PT_PARTNER_CP	=> false,
	FXP_PT_CONTACT_CP	=> false,
	FXP_PT_CLIENT_CP	=> false
);
if(fxIsArray($prs))
{
	if(isset($_POST['daten']) && is_array($_POST['daten']))
	{
		reset($_POST['daten']);
		$akey=key($_POST['daten']);
	}
	else
		$akey=$mc;

	foreach($prs as $p)
	{
		$id=$p['personen_id'];

		$nm=$p['pname'];
		$vn=trim($p['vorname']);
		if(strlen($vn))
			$nm .= ', '.$vn;

		$ld='0';
		$pt=$p['personentyp'];
		if(isset($inv[$id]))
		{
			$pt=34;	// Zugeordnet
			$ld='1';
			$p['termin_id']=$inv[$id]['termin_id'];
			$p['anmerkung']=$inv[$id]['anmerkung'];
		}
		if($id == $GLOBALS['fxpglobals']['person'])
			$pt=0;

		if($err)
		{
			if(fxIsArray($inv_list) && fxIsArray($inv_list[$id]))
			{
				if($inv_list[$id]['md'])
					$ld='1';
			}
			else
			{
				foreach($_POST['daten'][$akey] as $nr => $pv)
				{
					$pid=0;
					if(strlen($pv))
					{
						$pe=explode('|', $pv);
						$pid=(int)$pe[0];
					}

					if(($pid == $id) && $_POST['einladen'][$akey][$nr])
						$ld='1';
				}
			}
		}

		if(!is_array($lst[$pt]))
			$lst[$pt]=array();
		$lst[$pt][$id]=array(
			'pname'				=> $nm,
			'person_eingeladen'	=> $ld,
			'zusage'			=> substr($p['anmerkung'], 0, 1),
			'anmerkung'			=> substr($p['anmerkung'], 2),
			'daten'				=> $p['personen_id'].'|'.$p['termin_id'].'|'.$MasterTermin_ID,
			'geschlecht'		=> $p['geschlecht'],
			'email'				=> $p['e_mail_adresse'],
			'data'				=> false,
			'sondertage'		=> false,
			'termine'			=> false
		);

		// Arbeitstage holen
		$sql  = "SELECT maeintrittsdatum, maaustrittsdatum, wochenarbeitstd, arbeitstage, soll_beginn, pause, montag AS b, dienstag AS c, mittwoch AS d, donnerstag AS e, freitag AS f, samstag AS g, sonntag AS a";
		$sql .= " FROM madaten WHERE personen_id=".$p['personen_id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$mad=db_values($sql);
		if(fxIsArray($mad))
		{
//fxDebug($mad, $sql);
			$lst[$pt][$id]['data']=array();

			if($mad['arbeitstage'])
				$lst[$pt][$id]['data']['stundenprotag']=$mad['wochenarbeitstd']/$mad['arbeitstage'];
			else
				$lst[$pt][$id]['data']['stundenprotag']=0.00;
			if(strlen($mad['soll_beginn']))
				$lst[$pt][$id]['data']['start']=$mad['soll_beginn'];
			else
				$lst[$pt][$id]['data']['start']='00000000080000';
			$lst[$pt][$id]['data']['eintritt']=substr($mad['maeintrittsdatum'], 0, 8);
			if(strlen($mad['maaustrittsdatum']))
				$lst[$pt][$id]['data']['austritt']=substr($mad['maaustrittsdatum'], 0, 8);
			else
				$lst[$pt][$id]['data']['austritt']='99999999';
			for($m=0; $m<7; $m++)
			{
				$bchst=chr(0x61+$m);
				if($mad[$bchst])
					$lst[$pt][$id]['data'][$m]=1;
				else
					$lst[$pt][$id]['data'][$m]=0;
			}
		}

		// Sondertage holen
		$lst[$pt][$id]['sondertage']=get_sondertage($von, $bis, $p['personen_id']);

		// Termine holen
		$lst[$pt][$id]['termine']=get_termine($von, $bis, $p['personen_id']);
	}
}

// Personen anzeigen
if(fxIsArray($lst))
{
//fxDebug($lst, 'Personen-Liste');
	$cnt=0;
	if(($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE) ||  ($Termin_ID != $MasterTermin_ID))
	{
		$wta=array();
		$einladen_sf=2;
	}
	else
	{
		// Feiertage holen
		$feiertage=get_feiertage($von, $bis);
//fxDebug($feiertage, 'Feiertage');

		// Wochentage holen
		$sql="SELECT position, tabwert FROM wertetab WHERE id_feld=289 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")";
		$wta=db_values($sql,'*');
	}
	$wt=array();
	if(fxIsArray($wta))
	{
		foreach($wta as $v)
			$wt[$v['position']%7]=substr($v['tabwert'], 0, 2);

		// Überschriften
		$datum=$von;
		$kwtxt=meldung(1036);
		for($w=1; $w<4; $w++)
		{
			$kw=kw($datum);
			$GLOBALS['cwhead'.$w]='<b class="grey">'.$kwtxt.$kw.'</b> <i class="lightgrey normal">'.get_entry($datum, 'datum').'</i>';
			$datum=fx_date('Ymd', fx_mktime(0, 0, 0, (int)substr($datum, 4, 2), (int)substr($datum, 6, 2)+7, (int)substr($datum, 0, 4)));
		}

		// Nowrap
		$nv_text1_nw=true;
		$nv_text2_nw=true;
		$nv_text3_nw=true;
	}
	else
	{
		$nv_datum1_nv=true;
		$nv_datum2_nv=true;
		$nv_datum3_nv=true;

		$ordner_nv=true;
		$nv_text1_nv=true;
		$nv_text2_nv=true;
		$nv_text3_nv=true;
	}

	$heute=$GLOBALS['date'];

	fxf_maskColumnBorder(array('ordner'), 0, -1, $mc);

	// Alle Personengruppen
	$lgroup=0;
	$darr=array();
	foreach($lst as $group => $plst)
	{
		// Alle Personen in Gruppe
		if(fxIsArray($plst))
		{
			$invited=0;
			foreach($plst as $pid => $l)
			{
				if($l['person_eingeladen'] || $l['einladen'])
					$invited++;
			}

			if(!$group)
			{
				$gtxt='&nbsp;<b class="grey">'.get_text(21,'20').'</b>';	// Person

				$GLOBALS['fxptdata']['lvalues'][$mc][$cnt]['ordner']='~';
				$GLOBALS['fxptdata']['lvalues'][$mc][$cnt]['einladen']='^class_rhd" nowrap colspan="2"><!--|';
				$GLOBALS['fxptdata']['lvalues'][$mc][$cnt]['name_benutzer']='~-->'.$gtxt;
			}
			else
			{
				// Leerzeile mit Top Border?
				if($cnt)
				{
					if($lgroup)
						$GLOBALS['trinfo'.$mc.'_'.$cnt]='id="tr'.$lgroup.'_'.$pcnt.'" style="display:'.$disp.';"';
					fxf_maskColumnBorder(array('ordner|0', 'einladen', 'Name_Benutzer', 'nv_text1', 'nv_text2', 'nv_text3'), 1, -1, $mc, $cnt, true);
					$cnt++;
				}

				$actimg='fl_op';
				$disp='none';
				if($group == 34)
				{
					$actimg='fl_cl';
					$disp='';
				}
				$gtxt='&nbsp;<b class="grey">'.meldung($group).'</b>&nbsp;<i class="s1 grey normal">['.$invited.'/'.sizeof($plst).']</i>';

				$GLOBALS['fxptdata']['lvalues'][$mc][$cnt]['ordner']='~<img id="ipg'.$group.'" src="'.$GLOBALS['gfxpath'].$actimg.'.png" style="margin-top:4px;cursor:pointer;" '.fxf_jsFunction('toggleGroups', $group).'>';
				$GLOBALS['fxptdata']['lvalues'][$mc][$cnt]['einladen']='^class_rhd" nowrap colspan="2"><!--|';
				$GLOBALS['fxptdata']['lvalues'][$mc][$cnt]['name_benutzer']='~-->'.$gtxt;

				$lgroup=$group;
			}

			if(($GLOBALS['fxpglobals']['action'] == FXP_CREATE) || ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE))
			{
				$GLOBALS['fxptdata']['lvalues'][$mc][$cnt]['nv_text1']='^class_rhd" nowrap colspan="1|~'.$GLOBALS['cwhead1'];
				$GLOBALS['fxptdata']['lvalues'][$mc][$cnt]['nv_text2']='^class_rhd" nowrap colspan="1|~'.$GLOBALS['cwhead2'];
				$GLOBALS['fxptdata']['lvalues'][$mc][$cnt]['nv_text3']='^class_rhd" nowrap colspan="1|~'.$GLOBALS['cwhead3'];
			}

			$cnt++;

			$pcnt=0;
			foreach($plst as $pid => $l)
			{
				$GLOBALS['trinfo'.$mc.'_'.$cnt]='id="tr'.$group.'_'.$pcnt.'" style="display:'.$disp.';"';
				$darr[$group.'_'.$pcnt]=$cnt.'_'.$pid;

				$zi='';
				$gi='';
				if($l['zusage'] === '0')
				{
					$zi='0';
					$gi='red';
				}
				else if($l['zusage'] === '1')
				{
					$zi='1';
					$gi='green';
				}
				else if($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY)
					$gi='neutral';

				$GLOBALS['fxptdata']['lvalues'][$mc][$cnt]['ordner']='~';

				// Checkbox
				if(strlen($gi))
				{
					$GLOBALS['fxptdata']['lvalues'][$mc][$cnt]['einladen']='~<img src="'.$GLOBALS['gfxpath'].'/tl_'.$gi.'.png" style="margin-top:5px;">';
					if($zi == '1')
						$GLOBALS['fxptdata']['lvalues'][$mc][$cnt]['einladen'] .= fieldHidden('einladen['.$mc.']['.$cnt.']',1);
				}
				else if($l['person_eingeladen'] || $l['einladen'])
					$GLOBALS['fxptdata']['lvalues'][$mc][$cnt]['einladen']=1;

				// Name
				$GLOBALS['fxptdata']['lvalues'][$mc][$cnt]['name_benutzer']='~'.person_pic($pid, 20, $l['geschlecht'], $GLOBALS['fxpglobals']['client']).'&nbsp;&nbsp;<i class="grey">'.$l['email'].'</i>';
				if(strlen($zi) && $nv_text1_nv && strlen($l['anmerkung']))
					$GLOBALS['fxptdata']['lvalues'][$mc][$cnt]['name_benutzer'] .= '&nbsp;&nbsp;<i class="grey">{'.str_replace('&lt;', '<', $l['anmerkung']).'}</i>';

				// Die nächsten 3 Wochen anzeigen
				if(fxIsArray($wt))
				{
					$abw_text=meldung(185);
					$ter_text=meldung(186);
					$day=$stg;
					$datum=$von;
					for($w=1; $w<4; $w++)
					{
						$dt='^class_?" style="border-right:1px solid #c4c4c4;|~<table border="0" cellpadding="2" cellspacing="1"><tr>';
						for($t=0; $t<7; $t++)
						{
//echo('TAG: '.$day.' -> <B>DATUM:</B> '.$datum.'<BR>');

							// Hintergrund-Farbe
							$cl='';
							if($l['termine'][$datum])
								$cl='ccccff';					// Termin
							else if($datum == $heute)
								$cl='ccffff';					// Heute
							else if($feiertage[$datum])
								$cl='eeee88';					// Feiertag
							else if($l['sondertage'][$datum])
								$cl='ffff99';					// Sondertag
							else if(($l['data']['eintritt'] > $datum) || ($l['data']['austritt'] < $datum))
								$cl='dddddd';					// Undefiniert
							if(!strlen($cl))
							{
								if($l['data'][$day])
									$cl='ffffff';				// Arbeitstag
								else
									$cl='ffffbb';				// Frei
							}
							// Ausgewähltes Datum?
							$style='';
							if($datum == $sdatum)
								$style .='box-shadow:0 0 3px rgba(0,0,0, 0.75) inset;';

							// Tooltip
							$tooltip='';
							$tt_array=array();
							if($feiertage[$datum])				// Feiertag
							{
//fxDebug($feiertage[$datum], 'Feiertag: '.$datum);
								$tt_array['ft']=$feiertage[$datum]['name'];
							}
							if($l['sondertage'][$datum])		// Sondertag
							{
//fxDebug($l['sondertage'][$datum], 'Sondertag: '.$datum);
								$tt_array['st']=$l['sondertage'][$datum][-1]['dauer'];
							}
							if($l['termine'][$datum])			// Termine
							{
//fxDebug($l['termine'][$datum], 'Termin: '.$datum);
								foreach($l['termine'][$datum] as $tdt => $tvl)
								{
									$zt=get_entry($tvl['start'], 'zeit');
									if($tvl['ende'] != $tvl['start'])
										$zt .= ' - '.get_entry($tvl['ende'], 'zeit');
									$tt_array['tm'][]=$zt;
								}
							}
							if(fxIsArray($tt_array))
							{
//fxDebug($tt_array, 'Tooltip: '.$datum);
								$tooltip=' tooltip="<table width=100% border=0 cellpadding=1 cellspacing=';
								if($tt_array['st'] || is_array($tt_array['tm']))
									$tooltip .= '1';
								else
									$tooltip .= '0';
								$tooltip .= ' bgcolor=#dddddd>';
								$tooltip .= '<tr><td colspan=2 bgcolor=#dddddd align=center nowrap>&nbsp;<b>'.$wt[$day].'</b>,&nbsp;'.get_entry($datum, 'datum').'&nbsp;</td></tr>';
								if($tt_array['ft'])
									$tooltip .= '<tr><td colspan=2 bgcolor=#ffffff align=center nowrap>&nbsp;<font class=blue>'.$tt_array['ft'].'</font>&nbsp;</td></tr>';
								if($tt_array['st'])
									$tooltip .= '<tr><td bgcolor=#eeeeee>&nbsp;<b>'.$abw_text.':</b>&nbsp;</td><td bgcolor=#ffffff nowrap>&nbsp;'.get_entry($tt_array['st'], 'dezimal').'&nbsp;d&nbsp;</td></tr>';
								if(is_array($tt_array['tm']))
								{
									$tooltip .= '<tr><td bgcolor=#eeeeee>&nbsp;<b>'.$ter_text.':</b>&nbsp;</td><td bgcolor=#ffffff nowrap>';
									foreach($tt_array['tm'] as $tm)
										$tooltip .= '&nbsp;'.$tm.'&nbsp;<br>';
									$tooltip .= '</td></tr>';
								}
								$tooltip .= '</table>"';
								$style .= 'cursor:help;';
							}
							if(strlen($style))
								$style=' style="'.$style.'"';

							$dt .= '<td id="kw'.$w.'_'.$cnt.'" width="18" height="18" align="center" bgcolor="#'.$cl.'"'.$tooltip.$style.'>'.$wt[$day].'</td>';

							// Einen Tag hochzählen
							$ta=(int)substr($datum, 6, 2);
							$mo=(int)substr($datum, 4, 2);
							$ja=(int)substr($datum, 0, 4);
							$datum=fx_date('Ymd', fx_mktime(0, 0, 0, $mo, $ta+1, $ja));
							$day++;
							if($day > 6)
								$day=$day%7;
						}
						$GLOBALS['fxptdata']['lvalues'][$mc][$cnt]['nv_text'.$w]=$dt.'</tr></table>';
					}
				}

				// Hidden
				$GLOBALS['fxptdata']['lvalues'][$mc][$cnt]['daten']=$l['daten'];
				if(strlen($zi))
					$GLOBALS['fxptdata']['lvalues'][$mc][$cnt]['person_eingeladen']=$zi;
				else
					$GLOBALS['fxptdata']['lvalues'][$mc][$cnt]['person_eingeladen']=$l['person_eingeladen'];

				$pcnt++;
				$cnt++;
			}
		}
	}

	// Leerzeile mit Top Border?
	if($cnt)
	{
		fxf_maskColumnBorder(array('ordner|0', 'einladen', 'Name_Benutzer', 'nv_text1', 'nv_text2', 'nv_text3'), 1, -1, $mc, $cnt, true);
		$cnt++;
	}

	if(fxIsArray($darr))
	{
//fxDebug($darr, '$darr');
		$trids='';
		$peids='';
		foreach($darr as $trid => $peid)
		{
			if(strlen($trids))
			{
				$trids .= '|';
				$peids .= '|';
			}
			$trids .= $trid;
			$peids .= $peid;
		}
		echo('<div id="trids" style="display:none;">'.$trids.'</div><div id="peids" style="display:none;">'.$peids.'</div>');
	}
}
else
	$GLOBALS['fxptdata']['lvalues'][$mc]=true;
?>