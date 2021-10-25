<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : ekvk_eingabe_pre.inc                                         //
// Version     : 21.2                                                         //
// Begin       : 2020-11-17                                                   //
// Last Change : 2021-10-06                                                   //
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
 * 268: Hourly Rates
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$index521=true;
$mc=$GLOBALS['_maskcounter'];
$l=0;
$kat=(int)$_POST['Kategorie'];
$rid=$_POST['AuswahlFeldtyp'];
$sp=(int)$_POST['MaArt_ID'];

$mca=array('Feldname', 'typ', 'EK', 'VK', 'Loeschen');

$pia=explode('_', $rid);
if(sizeof($pia) == 1)
{
	$id=(int)$pia[0];
	$rid=(int)$rid;
}
else
	$id=(int)$pia[1];

// Texte
$ssatz=get_text(104, '20');	// Stundensatz
$ssatz_std=$ssatz.'&nbsp;('.meldung(1700, false).')';	// Stundensatz (Standard)
$ssatz_dlt=$ssatz.'&nbsp;('.get_text(915, '5').')';		// Stundensatz (Delta)
$ssatz_fg=$ssatz.'&nbsp;('.get_text(373, '20').')';		// Stundensatz (Fähigkeitsgruppe)
$ssatz_ff=$ssatz.'&nbsp;('.get_text(375, '20').')';		// Stundensatz (Fähigkeit)
$ssatz_pr=$ssatz.'&nbsp;('.get_text(66, '20').')';		// Stundensatz (Projekt)

// Nowrap-Felder
$Feldname_nw=true;
$typ_nw=true;

// Fähigkeiten, Personen oder Projekte ermitteln
$pmode=false;
if(($kat == 66) && ($sp < 0))		// Projekt + Ohne Fähigkeiten -> Personen
	$pmode=true;

$farr=false;
if($sp >= 0)		// Fähigkeiten
	$farr=auswahl_maart_id(1);
else if($kat == 66)	// Personen
{
	$parr=auswahl_Personen_ID(1);
	if(fxIsArray($parr))
	{
		foreach($parr as $pid => $ptxt)
		{
			$pop=strpos($ptxt,'] (');
			if($pop !== false)
				$ptxt=substr($ptxt,0,$pop+2).'<i class="grey">'.substr($ptxt,$pop+2).'</i>';
			if(strpos($ptxt,'<red>') !== false)
				$ptxt='<i class="red">'.str_replace('<red>','',$ptxt).'</i>';
			$farr[-$pid]=$ptxt;
		}
	}
}
else				// Projekte
{
	$pt=(int)db_value("SELECT personentyp FROM personen WHERE personen_id=".$id." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
	if(($pt == FXP_PT_EMPLOYEE) || ($pt == FXP_PT_CONTRACTOR))
	{
		$parr=auswahl_Mutterprojekt_ID(1);
		if(fxIsArray($parr))
		{
			foreach($parr as $pid => $ptxt)
			{
				$ptxt=trim(str_replace('{**}','',$ptxt));
				$farr[-$pid]=$ptxt;
			}
		}
	}
}
//fxDebug($farr,'$farr', 0);

// Fähigkeiten-Analyse
$sg=-666;
$sf=-666;
if($sp > 0)
{
	if(substr($_POST['MaArt_ID'], -1) == 'f')
		$sf=$sp;
	else
		$sg=$sp;
}
$lg=0;
$lf[0]=0;
$fg=0;
$ff=0;
if(fxIsArray($farr))
{
	foreach($farr as $fvalue => $ftext)
	{
		if(substr($fvalue, -1) == 'f')
			$ff=(int)$fvalue;
		else
		{
			$fg=(int)$fvalue;
			$ff=0;
		}

		// Letzte Einträge
		$lg=$fg;
		$lf[$lg]=$ff;

		// Ausgewählte Fähigkeit
		if($sf == $ff)
			$sg=$fg;
	}
}
if($sg > 0)
{
	$lg=$sg;
	$lf=array($lg => $lf[$lg]);

	if($sf > 0)
		$lf[$lg]=$sf;
}
//fxDebug($lf, $lg, 0);

// Bestand laden
$ekvk=array();
if($pmode)
	$sql="SELECT * FROM ekvk WHERE feldtyp=66 AND id=".$id." AND maart_id<0 AND mafaehigkeit_id<0 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
else
	$sql="SELECT * FROM ekvk WHERE feldtyp=".$kat." AND id=".$id." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
$tmp=db_values($sql, '*');
//fxDebug($tmp,$sql, 0);
if(fxIsArray($tmp))
{
	foreach($tmp as $t)
	{
		$pk=(int)$t['maart_id'].'|'.(int)$t['mafaehigkeit_id'];
		$ekvk[$pk]=$t;
	}
}
//fxDebug($ekvk,$sql, 0);

// Standard-Stundensatz
$ael=false;
$ssatz=$ssatz_std;

$pk='0|0';

$ek='';
$vk='';
$cb='';

// Bei Int. + Ext. Mitarbeitern Standardstundensatz aus "Artikel" ermitteln
if($kat == 21)
{
	$sql="SELECT personentyp FROM personen WHERE personen_id=".$id." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$ptyp=(int)db_value($sql);
	if(($ptyp == FXP_PT_EMPLOYEE) || ($ptyp == FXP_PT_CONTRACTOR))
	{
		$sql="SELECT ek, vk FROM artikel WHERE personen_id=".$id." AND (sachmittel_id=0 OR sachmittel_id IS NULL) AND (archiv=0 OR archiv IS NULL) AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$pev=db_values($sql);

		$ek='~<i class="darkgrey">'.get_entry($pev['ek'], 'dezimal').'</i>';
		$vk='~<i class="darkgrey">'.get_entry($pev['vk'], 'dezimal').'</i>';
		$cb='~';

		$pk='';
	}
}
// Durchschnittsstundensatz bei Projekten
else
{
	$ssatz=$ssatz_dlt;
	$rarr[$rid]=trim(strtr($rarr[$rid],array('{**}'=>'', '{*}'=>'')));

	$vn=db_value("SELECT vorgangsnummer FROM projekte WHERE projekt_id=".$id." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
	$sql  = "SELECT m.aufwand_std, a.ek_projekt, a.vk_projekt FROM maplanung m, artikelzuordnung a";
	$sql .= " WHERE m.projekt_id IN (SELECT projekt_id FROM projekte WHERE projektart=".FXP_PRJ_TASK." AND vorgangsnummer LIKE '".$vn.".%') AND m.aufwand_std>0 AND m.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$sql .= " AND a.maplanung_id=m.maplanung_id AND a.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql, 0);

	$dek=0.00;
	$dvk=0.00;
	if(fxIsArray($tmp))
	{
		$das=0;
		foreach($tmp as $t)
		{
			$das += $t['aufwand_std'];
			$dek += $t['aufwand_std']*$t['ek_projekt'];
			$dvk += $t['aufwand_std']*$t['vk_projekt'];
		}
		$dek /= $das;
		$dvk /= $das;
	}

	$ek='~<i class="darkgrey">&Delta; '.get_entry($dek, 'dezimal').'</i>';
	$vk='~<i class="darkgrey">&Delta; '.get_entry($dvk, 'dezimal').'</i>';
	$cb='~';

	$pk='';
}

if(!strlen($ek))
{
	if($err && fxIsArray($err_array) && fxIsArray($err_array[$pk]))
	{
		$ek=$err_array[$pk]['ek'];
		$vk=$err_array[$pk]['vk'];
		$cb=$err_array[$pk]['cb'];
	}
	else if(fxIsArray($ekvk) && fxIsArray($ekvk[$pk]))
	{
		$ek=$ekvk[$pk]['ek'];
		$vk=$ekvk[$pk]['vk'];
	}
}

// ...Hauptwerte
$cls='^col_d3e0e6|';
$ra=extractTooltip($rarr[$rid]);
$GLOBALS['fxptdata']['lvalues'][$mc][$l]['feldname']=$cls.'~<div style="padding:4px;font-weight:bold;'.$ra['tooltip2'].'">'.$ra['text'].'</div>';
$GLOBALS['fxptdata']['lvalues'][$mc][$l]['typ']=$cls.'~&nbsp;<font class="darkgreen">'.$ssatz.'</font>&nbsp;';
$GLOBALS['fxptdata']['lvalues'][$mc][$l]['ek']=$cls.$ek;
$GLOBALS['fxptdata']['lvalues'][$mc][$l]['vk']=$cls.$vk;
$GLOBALS['fxptdata']['lvalues'][$mc][$l]['loeschen']=$cls.$cb;

// ...Hidden
$GLOBALS['fxptdata']['lvalues'][$mc][$l]['feldtyp']=$kat;
$GLOBALS['fxptdata']['lvalues'][$mc][$l]['id']=$id;
$GLOBALS['fxptdata']['lvalues'][$mc][$l]['ma_art_faehigkeit_id']=0;
$GLOBALS['fxptdata']['lvalues'][$mc][$l]['mafaehigkeit_id']=0;
$GLOBALS['fxptdata']['lvalues'][$mc][$l]['nv_text1']='';
$GLOBALS['fxptdata']['lvalues'][$mc][$l]['nv_text2']=strtr($rarr[$rid], $caption_strtr);
$GLOBALS['fxptdata']['lvalues'][$mc][$l]['nv_betrag1']='';
$GLOBALS['fxptdata']['lvalues'][$mc][$l]['nv_betrag2']='';

if(strlen($pk) && fxIsArray($ekvk) && fxIsArray($ekvk[$pk]))
{
	$GLOBALS['fxptdata']['lvalues'][$mc][$l]['nv_text1']=$pk;
	$GLOBALS['fxptdata']['lvalues'][$mc][$l]['nv_betrag1']=trim($ekvk[$pk]['ek']);
	$GLOBALS['fxptdata']['lvalues'][$mc][$l]['nv_betrag2']=trim($ekvk[$pk]['vk']);
}

$l++;
$ael=true;

// Fähigkeiten-Stundensatz
if(fxIsArray($farr))
{
	$pfa=array();
	if(($kat == 21) && !$pmode)
	{
		$pt=db_value("SELECT personentyp FROM personen WHERE personen_id=".$id." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
		if(($pt == FXP_PT_EMPLOYEE) || ($pt == FXP_PT_CONTRACTOR))
			$pfa[0]=0;

		$sql="SELECT maart_id,mafaehigkeit_id FROM ma_art_faehigkeiten WHERE personen_id=".$id." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql, 0);
		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
				$pfa[$t['mafaehigkeit_id']]=$t['maart_id'];
		}
	}
//fxDebug($pfa,'$pfa', 0);

	$vfa=$farr;

	$fg=0;
	$ff=0;
	foreach($farr as $fvalue => $ftext)
	{
		if($pmode)
		{
			if($kat == 21)
			{
				$fg=$fvalue;
				$ff=-$id;
			}
			else
			{
				$fg=-$id;
				$ff=$fvalue;
			}
			$pk=$fg.'|'.$ff;
		}
		else
		{
			if(substr($fvalue,-1) == 'f')
				$ff=(int)$fvalue;
			else
			{
				$fg=(int)$fvalue;
				$ff=0;
			}

			if($ff)
				$pk='0|'.$ff;
			else
				$pk=$fg.'|0';
		}
//echo('$fvalue='.$fvalue.': $sg='.$sg.', $sf='.$sf.' - $fg='.$fg.', $ff='.$ff.' - $pk='.$pk.'<br />');

		$vfa[$fvalue]=false;
		if($ff)
		{
			if(($sf == $ff) || ($sg == $fg) || (($sf == -666) && ($sg == -666) && (!sizeof($pfa) || (sizeof($pfa) && (((fxIsArray($ekvk) && fxIsArray($ekvk[$pk])) || isset($pfa[$ff])))))))
			{
				$vfa[$fvalue]=true;
				$vfa[$fg]=true;
			}
		}
		else if(($sg == $fg) || (($sf == -666) && ($sg == -666) && !sizeof($pfa)))
			$vfa[$fvalue]=true;
	}
//fxDebug($vfa,'$vfa', 0);

	$fg=0;
	$ff=0;
	$el=false;
	$hl='';
	$ml=-1;
	foreach($farr as $fvalue => $ftext)
	{
		$skip=false;
		if(!$el && ($ael || ($ftext == '<d>')))
		{
			$el=true;
			$ael=false;

			if($ftext == '<d>')
				$skip=true;
		}

		// Stundensatzzeile?
		if(!$skip)
		{
			if(substr($fvalue,-1) == 'f')
				$ff=(int)$fvalue;
			else
			{
				$fg=(int)$fvalue;
				$ff=0;
			}

			// Nur Fähigkeiten anzeigen, welche den ausgewählten Kriterien entsprechen
			if($vfa[$fvalue])
			{
				// Leerzeile?
				if(!$ff && $l)
				{
					fxf_maskColumnBorder($mca, 1, -1, $mc, $l, true);
					$l++;
				}

				$cls='';
				if(!$ff)
					$cls='^col_e8e8e9|';

				$el=false;

				$hkat=$kat;
				$hid=$id;

				if($pmode)
				{
					if($kat == 21)
					{
						$fg=$fvalue;
						$ff=-$id;
					}
					else
					{
						$fg=-$id;
						$ff=$fvalue;
					}
					$pk=$fg.'|'.$ff;

					$hkat=66;
					$hid=abs($fg);
				}
				else if($ff)
					$pk='0|'.$ff;
				else
					$pk=$fg.'|0';

				$ek='';
				$vk='';
				$cb='';
				if($err && fxIsArray($err_array) && fxIsArray($err_array[$pk]))
				{
					$ek=$err_array[$pk]['ek'];
					$vk=$err_array[$pk]['vk'];
					$cb=$err_array[$pk]['cb'];
				}
				else if(fxIsArray($ekvk) && fxIsArray($ekvk[$pk]))
				{
					$ek=$ekvk[$pk]['ek'];
					$vk=$ekvk[$pk]['vk'];
				}

				// Projekt oder Person
				$ra=extractTooltip($ftext,true);
				if($pmode)
				{
					$feldname='~&nbsp;<font class="black"'.$ra['tooltip2'].'>'.$ra['text'].'</font>&nbsp;';
					$typ='~&nbsp;<font class="blue">'.$ssatz_pr.'</font>&nbsp;';
				}
				// Fähigkeitsgruppe
				else if(!$ff)
				{
					$feldname='~&nbsp;<font class="black"'.$ra['tooltip2'].'>'.$ra['text'].'</font>&nbsp;';
					$typ='~&nbsp;<font class="blue">'.$ssatz_fg.'</font>&nbsp;';
				}
				// Fähigkeit
				else
				{
					$feldname='~&nbsp;&nbsp;&nbsp;<font class="darkgrey"'.$ra['tooltip2'].'>'.$ra['text'].'</font>&nbsp;';
					$typ='~&nbsp;<font class="lightblue">'.$ssatz_ff.'</font>&nbsp;';
				}

				// ...Hauptwerte
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['feldname']=$cls.$feldname;
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['typ']=$cls.$typ;
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['ek']=$cls.$ek;
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['vk']=$cls.$vk;
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['loeschen']=$cls.$cb;

				// ...Hidden
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['feldtyp']=$hkat;
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['id']=$hid;
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['ma_art_faehigkeit_id']=$fg;
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['mafaehigkeit_id']=$ff;
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['nv_text1']='';
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['nv_text2']=strtr($ftext, $caption_strtr);
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['nv_betrag1']='';
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['nv_betrag2']='';

				if(!$pmode)
				{
					if($ff)
						$GLOBALS['fxptdata']['lvalues'][$mc][$l]['ma_art_faehigkeit_id']=0;
					else
						$GLOBALS['fxptdata']['lvalues'][$mc][$l]['mafaehigkeit_id']=0;
				}

				if(fxIsArray($ekvk) && fxIsArray($ekvk[$pk]))
				{
					$GLOBALS['fxptdata']['lvalues'][$mc][$l]['nv_text1']=$pk;
					$GLOBALS['fxptdata']['lvalues'][$mc][$l]['nv_betrag1']=trim($ekvk[$pk]['ek']);
					$GLOBALS['fxptdata']['lvalues'][$mc][$l]['nv_betrag2']=trim($ekvk[$pk]['vk']);
				}

				$l++;
			}
		}
	}

	if($l)
	{
		fxf_maskColumnBorder($mca, 1, -1, $mc, $l, true);
		$l++;
	}
}
?>