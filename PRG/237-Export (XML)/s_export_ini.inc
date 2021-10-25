<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : s_export_ini.inc                                             //
// Version     : 21.2                                                         //
// Begin       : 2020-12-17                                                   //
// Last Change : 2021-04-15                                                   //
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
 * PF 237: Export (XML)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

// We need our XML function collection
$req_fn=fxf_fn_reqFilename('s_exim_tools', 237);
if(strlen($req_fn))
	require($req_fn);

$GLOBALS['_masknumber']=476;
setForm(true);

// Standard-Maskenbreite
$mwidth=(int)db_value("SELECT breite FROM masken WHERE nummer=476 AND mandanten_id=0");
if(!$mwidth)
	$mwidth=940;

// Default-Werte
$def_step=1;
$def_group=0;
$def_ptyp=array(276=>'1', 620=>'1', 2091=>'1', 2092=>'1', 275=>'1', 840=>'1', 1372=>'1', 841=>'1');
$def_padd=array(578=>'1', 579=>'1');

// POST-VARS
if(!fxIsArray($_POST) || !$_POST['_step'] || (fxIsArray($_GET) && ($_GET['tr'] == $GLOBALS['fxpglobals']['tr'])))
	$_step=$def_step;
else
	$_step=(int)$_POST['_step'];

if(($GLOBALS['fxpglobals']['tr'] == 237) || ($GLOBALS['fxpglobals']['tr'] == 239))
	$rstep=$_step+1;
else
	$rstep=$_step;
//echo('<b>$_step:</b> '.$_step.', <b>$rstep:</b> '.$rstep.'<br />');

if(($GLOBALS['fxpglobals']['tr'] == 237) || ($GLOBALS['fxpglobals']['tr'] == 238))
	$_group=574;	// Personen
else if(($GLOBALS['fxpglobals']['tr'] == 239) || ($GLOBALS['fxpglobals']['tr'] == 240))
	$_group=575;	// Projekte
else if(fxIsArray($_POST) && $_POST['_group'])
	$_group=(int)$_POST['_group'];
else
	$_group=$def_group;

if(fxIsArray($_POST) && fxIsArray($_POST['_ptyp']))
{
	foreach($_POST['_ptyp'] as $id => $vl)
		$_ptyp[$id]=$vl;
}
else
	$_ptyp=$def_ptyp;
//fxDebug($_ptyp, '$_ptyp');

if(fxIsArray($_POST['_padd']))
{
	foreach($_POST['_padd'] as $id => $vl)
		$_padd[$id]=$vl;
}
else
	$_padd=$def_padd;
//fxDebug($_padd, '$_padd');

// Nächsten Schritt ermitteln...
if(!$err)
{
	if($_POST['Button_Next'] || $_POST['Button_XML'])
		$_step++;
	if($_POST['Button_Back'])
		$_step--;
	if($_POST['Button_Start'])
		$_step=1;
}

if(($GLOBALS['fxpglobals']['tr'] == 237) || ($GLOBALS['fxpglobals']['tr'] == 239))
	$rstep=$_step+1;
else
	$rstep=$_step;

// Texte
$txt_schritt=meldung(573, false);

// Export-Schritte einladen
if($GLOBALS['fxpglobals']['tr'] != 235)
	$startid='';
else
	$startid='568,';
$steps=array();
$sql="SELECT tabwert FROM wertetab WHERE mandanten_id=0 AND id_feld=255 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND wertetabid IN (".$startid."569,570,571,572) ORDER BY position";
$tmp=db_values($sql, '*');
if(fxIsArray($tmp))
{
	$sc=1;
	foreach($tmp as $t)
		$steps[$sc++]=str_replace('ISO-8859-1', FXP_CHARSET, $t['tabwert']);
}

// Inhalt
$inhalt='';
switch($rstep)
{
	case 1:	// Datengruppe auswählen
		$txt_datengruppe=meldung(577, false);	// Welche Datengruppe exportieren?

		$sar=array();
		$sql="SELECT wertetabid, tabwert FROM wertetab WHERE mandanten_id=0 AND id_feld=1599 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." ORDER BY position";
		$arr=db_values($sql, '*');
		if(fxIsArray($arr))
		{
			foreach($arr as $a)
				$sar[$a['wertetabid']]=$a['tabwert'];
		}
		$GLOBALS['_group_ds_array']=$sar;

		$inhalt .= '<table width="'.($mwidth-20).'" border="0" cellpadding="0" cellspacing="0">'.$nl;
		$inhalt .= '	<tr>'.$nl;
		$inhalt .= '		<td nowrap width="'.($mwidth/3).'"><span class="lit">'.$txt_datengruppe.'</span></td>'.$nl;
		$inhalt .= '		<td nowrap>'.fieldSelect('_group', $_group, $mwidth*2/3-20).'</td>'.$nl;
		$inhalt .= '	</tr>'.$nl;
		$inhalt .= '</table>'.$nl;
	break;

	case 2:	// Auswahlkriterien festlegen
		$txt_personentyp=get_text(266, '20');	// Personen-Typ
		$txt_zusatz=meldung(581, false);		// Zusatz

		// Personentypen ermitteln
		$ptp=array();
		$sql="SELECT wertetabid, tabwert FROM wertetab WHERE mandanten_id=0 AND id_feld=30 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND wertetabid NOT IN (274,2090) ORDER BY position";
		$arr=db_values($sql, '*');
		if(is_array($arr))
		{
			foreach($arr as $a)
				$ptp[$a['wertetabid']]=$a['tabwert'];
		}

		// Personen Zusatzauswahl ermitteln
		$pad=array();
		$sql="SELECT wertetabid, tabwert FROM wertetab WHERE mandanten_id=0 AND wertetabid IN (578,579,580) AND id_sprache=".$GLOBALS['fxpglobals']['lang']." ORDER BY position";
		$arr=db_values($sql, '*');
		if(is_array($arr))
		{
			foreach($arr as $a)
				$pad[$a['wertetabid']]=$a['tabwert'];
		}

		$inhalt .= '<table width="'.($mwidth-20).'" border="0" cellpadding="0" cellspacing="0">'.$nl;
		$inhalt .= '	<tr>'.$nl;
		$inhalt .= '		<td nowrap width="15%" valign="top"><span class="lit">'.$txt_personentyp.':</span></td>'.$nl;
		$inhalt .= '		<td nowrap width="35%" valign="top">'.sys_check('_ptyp', $ptp).'</td>'.$nl;
		$inhalt .= '		<td nowrap width="15%" valign="top"><span class="lit">'.$txt_zusatz.':</span></td>'.$nl;
		$inhalt .= '		<td nowrap width="35%" valign="top">'.sys_check('_padd', $pad).'</td>'.$nl;
		$inhalt .= '	</tr>'.$nl;
		$inhalt .= '</table>'.$nl;
	break;

	case 3:	// Elementnamen festlegen
		$table=array('personen');
		if($_padd[578])
			$table[]='adressen';
		if($_padd[579])
			$table[]='kommunikation';

		$felder=load_exim_cfg($_group);
		if(fxIsArray($felder))
		{
			$z=0;
//fxDebug($felder, 'Felder');
			$inhalt .= '<table width="'.($mwidth-20).'" border="0" cellpadding="0" cellspacing="0">'.$nl;
			foreach($table as $tabname)
			{
				if($z)
				{
					$inhalt .= '	<tr>'.$nl;
					$inhalt .= '		<td>&nbsp;</td>'.$nl;
					$inhalt .= '		<td colspan="3" style="border-top:1px solid #'.$GLOBALS['msk_nbcol'].';">&nbsp;</td>'.$nl;
					$inhalt .= '	</tr>'.$nl;
				}
				foreach($felder as $feld => $farr)
				{
					if($farr['tab'] != $tabname)
						continue;

					$lcls='lit';
					if($farr['art'] == '*')
					{
						$img='key_b_15x15';
						$lcls='bold black';
					}
					else if($farr['art'] == '+')
					{
						$img='tl_green';
						$lcls='black';
					}
					else
						$img='tl_neutral';

					$cls=' class="rv'.(($z%2)+1).'"';

					$inhalt .= '	<tr>'.$nl;
					$inhalt .= '		<td align="center" nowrap><img src="'.$GLOBALS['gfxpath'].$img.'.png">'.fieldHidden('_xmlid['.$z.']',$farr['id']).'&nbsp;</td>'.$nl;
					$inhalt .= '		<td'.$cls.' width="100%" nowrap style="padding:2px;padding-right:0;border-right:1px solid #'.$GLOBALS['msk_nbcol'].';"><span id="dummy" class="'.$lcls.'" tooltip="'.$farr['text'].'" style="cursor:help;">&nbsp;'.$farr['name'].'&nbsp;</span></td>'.$nl;
					$inhalt .= '		<td'.$cls.' nowrap style="padding:2px;padding-right:0;border-right:1px solid #'.$GLOBALS['msk_nbcol'].';">'.fieldText('_xml['.$z.']',$farr['xml'],$mwidth/4,255,'left',true).'&nbsp;</td>'.$nl;
					$inhalt .= '		<td'.$cls.' nowrap style="padding:2px;padding-right:0;border-right:1px solid #'.$GLOBALS['msk_nbcol'].';">&nbsp;'.fieldText('_xmlalias['.$z.']',str_replace('|', ', ', $farr['alias']),$mwidth/4,255).'</td>'.$nl;
					$inhalt .= '	</tr>'.$nl;

					$z++;
				}
			}
			if($z)
			{
				$inhalt .= '	<tr>'.$nl;
				$inhalt .= '		<td>&nbsp;</td>'.$nl;
				$inhalt .= '		<td colspan="3" style="border-top:1px solid #'.$GLOBALS['msk_nbcol'].';">&nbsp;</td>'.$nl;
				$inhalt .= '	</tr>'.$nl;
			}
			$inhalt .= '</table>'.$nl;
		}
	break;

	case 4:	// Datenbank -> XML
		// Elementnamen speichern
		if($_POST['Button_Next'])
		{
			$felder=load_exim_cfg($_group);
			if(fxIsArray($felder))
			{
				$sr=array(', '=>'|', ','=>'|', ' '=>'_', '"'=>'', "'"=>'');
				$sf='';
				foreach($felder as $feld => $farr)
				{
					$zfound=-1;
					foreach($_POST['_xmlid'] as $z => $id)
					{
						if($farr['id'] == $id)
						{
							$zfound=$z;
							break;
						}
					}

					if($zfound > -1)
					{
						$xml=strtr(trim($_POST['_xml'][$z]), $sr);
						$alias=strtr(trim($_POST['_xmlalias'][$z]), $sr);
					}
					else
					{
						$xml=$farr['xml'];
						$alias=$farr['alias'];
					}

					$sf .= $farr['tab'].'^'.$farr['art'].$farr['name'].'^'.$farr['id'].'^'.$farr['typ'];
					if($farr['def'] !== false)
						$sf .= '_'.$farr['def'];
					$sf .= '^'.$xml.'^'.$alias.'^'.$farr['text'].'^'.$farr['bsp'].$GLOBALS['nl_b'];
				}
				save_exim_cfg($_group, $sf);
			}
		}

		// Liste aus DB holen
		$sql="SELECT personen_id, personentyp, geschlecht, pname, vorname FROM personen WHERE mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$pt='';
		if(fxIsArray($_ptyp))
		{
			foreach($_ptyp as $ptid => $value)
			{
				if($value)
					$pt .= ','.$ptid;
			}
		}

		// Nur Personen, auf die Rechte vorhanden sind
		$persrights=get_persons();
		if(strlen($persrights))
			$sql .= " AND personen_id IN (".$persrights.")";
		if(strlen($pt))
			$sql .= " AND personentyp IN (".substr($pt, 1).")";
		if(!$_padd['580'])	// Keine inakt. Personen
			$sql .= " AND (inaktiv IS NULL OR inaktiv=0)";
		$sql .= "	ORDER BY pname, vorname";
		$pl=db_values($sql, '*');
		if(fxIsArray($pl))
		{
//fxDebug($pl, $sql);
			$ptl=$_ptyp;
			if(!is_array($ptl))
				$ptl=array(276=>'1', 620=>'1', 2091=>'1', 2092=>'1', 275=>'1', 840=>'1', 1372=>'1', 841=>'1');

			$ptchk=-1;
			$z=0;
			$adr=array();
			$cs=2;
			if($_padd[578])
			{
				$cs++;
				$sql="SELECT DISTINCT a.personen_id, a.strasse, w.tabwert, a.plz, a.ort FROM adressen a, wertetab w WHERE w.wertetabid=a.landkz AND w.id_sprache=".$GLOBALS['fxpglobals']['lang']." AND a.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND w.mandanten_id=0";
				$ada=db_values($sql, '*');
				if(fxIsArray($ada))
				{
					foreach($ada as $a)
						$adr[$a['personen_id']]='&nbsp;'.$a['strasse'].'&nbsp;<br />&nbsp;'.$a['tabwert'].'-'.$a['plz'].'&nbsp;'.$a['ort'].'&nbsp;';
				}
			}

			$inhalt .= '<table width="'.($mwidth-20).'" border="0" cellpadding="0" cellspacing="0">'.$nl;
			foreach($ptl as $ptid => $disp)
			{
				if($disp)
				{
					if($ptid && ($ptid != $ptchk))
					{
						$ptchk=$ptid;
						if($z)
						{
							$inhalt .= '	<tr>'.$nl;
							$inhalt .= '		<td colspan="'.$cs.'" style="border-top:1px solid #'.$GLOBALS['msk_nbcol'].';">&nbsp;</td>'.$nl;
							$inhalt .= '	</tr>'.$nl;
						}
						$inhalt .= '	<tr>'.$nl;
						$inhalt .= '		<td class="maskt" colspan="'.$cs.'" nowrap>&nbsp;'.meldung($ptid, false).'</td>'.$nl;
						$inhalt .= '	</tr>'.$nl;
					}

					foreach($pl as $p)
					{
						if($ptid && ($p['personentyp'] != $ptid))
							continue;

						$cls=' class="rv'.(($z%2)+1).'"';

						$inhalt .= '	<tr>'.$nl;
						$inhalt .= '		<td'.$cls.' valign="top" style="border-right:1px solid #'.$GLOBALS['msk_nbcol'].';">'.fieldCheckbox('_pers['.$p['personen_id'].']','',1).'</td>'.$nl;
						$inhalt .= '		<td'.$cls.' width="100%" valign="top" nowrap style="border-right:1px solid #'.$GLOBALS['msk_nbcol'].';">&nbsp;'.person_pic($p['personen_id']).'&nbsp;</td>'.$nl;
						if($_padd[578])	// Adresse
							$inhalt .= '		<td'.$cls.' valign="top" nowrap style="border-right:1px solid #'.$GLOBALS['msk_nbcol'].';"><span class="grey">'.$adr[$p['personen_id']].'</span></td>'.$nl;
						$inhalt .= '	</tr>'.$nl;

						$z++;
					}
				}
			}
			if($z)
			{
				$inhalt .= '	<tr>'.$nl;
				$inhalt .= '		<td colspan="'.$cs.'" style="border-top:1px solid #'.$GLOBALS['msk_nbcol'].';">&nbsp;</td>'.$nl;
				$inhalt .= '	</tr>'.$nl;
			}
			$inhalt .= '</table>'.$nl;
		}
	break;

	case 5:	// Ergebnis
		$ps='';
		if(fxIsArray($_POST['_pers']))
		{
			foreach($_POST['_pers'] as $pid => $set)
			{
				if($set)
					$ps .= ','.$pid;
			}
		}
		$ps=substr($ps, 1);

		$pdat=array();
		$adat=array();
		$kdat=array();
		if(strlen($ps))
		{
			// Alle Infos ermitteln
			$sql="SELECT * FROM personen WHERE personen_id IN (".$ps.") AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY personen_id";
			$tmp=db_values($sql, '*');
			if(fxIsArray($tmp))
			{
				foreach($tmp as $t)
					$pdat[$t['personen_id']]=$t;
			}
			if($_padd[578])	// Adressen
			{
				$sql="SELECT * FROM adressen WHERE personen_id IN (".$ps.") AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY adressen_id";
				$tmp=db_values($sql, '*');
				if(fxIsArray($tmp))
				{
					foreach($tmp as $t)
						$adat[$t['personen_id']][]=$t;
				}
			}
			if($_padd[579])	// Kommunikation
			{
				$sql="SELECT * FROM kommunikation WHERE personen_id IN (".$ps.") AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY kommunikation_id";
				$tmp=db_values($sql, '*');
				if(fxIsArray($tmp))
				{
					foreach($tmp as $t)
						$kdat[$t['personen_id']][]=$t;
				}
			}
		}

		$fn='fx_export_persons_'.time().'_'.$GLOBALS['fxpglobals']['person'].'.xml';
		$fp=$GLOBALS['tmppath'].$fn;
		$xml='';
		if(fxIsArray($pdat))
		{
			$felder=load_exim_cfg($_group);

			$xml .= "<?xml version='1.0' encoding='".FXP_CHARSET."' standalone='yes' ?>".$GLOBALS['nl_b'];
/*
			$xml .= "<!DOCTYPE list [".$GLOBALS['nl_b'];
			$xml .= "	<!ATTLIST list".$GLOBALS['nl_b'];
			$xml .= "		referer CDATA #IMPLIED".$GLOBALS['nl_b'];
			$xml .= "	>".$GLOBALS['nl_b'].$GLOBALS['nl_b'];
*/

			$elem=array();
			$main='';
			foreach($felder as $feldname => $data)
			{
				$tname=$data['tab'];
				if(!$main)
					$main=$tname;
				if(!isset($elem[$tname]))
					$elem[$tname]=array();
				$elem[$tname][$data['xml']]=$data['art'];
				if($tname != $main)
				{
					if(!isset($elem[$main]))
						$elem[$main]=array();
					$elem[$main][$tname]='#';
				}
			}
/*
			foreach($elem as $elname => $eldata)
			{
				$ellst='';
				$elatr='';
				$eldef='';
				foreach($eldata as $elsub => $elval)
				{
					if($elval == '*')
					{
						if($elatr)
							$elatr .= ",".$GLOBALS['nl_b'];
						$elatr .= "		".$elsub." CDATA #IMPLIED";
					}
					else
					{
						if($ellst)
							$ellst .= ", ";
						$ellst .= $elsub;
						if($elval == '-')
							$ellst .= "?";
						else if($elval == '#')
							$ellst .= "*";

						if($elval != '#')
						{
							if($eldef)
								$eldef .= $GLOBALS['nl_b'];
							$eldef .= "	<!ELEMENT $elsub (#PCDATA)>";
						}
					}
				}
				if($ellst)
					$xml .= "	<!ELEMENT $elname (".$GLOBALS['nl_b']."		".$ellst.$GLOBALS['nl_b']."	)+>".$GLOBALS['nl_b'];
				if($elatr)
					$xml .= "	<!ATTLIST $elname".$GLOBALS['nl_b'].$elatr.$GLOBALS['nl_b']."	>".$GLOBALS['nl_b'];
				if($eldef)
					$xml .= $eldef.$GLOBALS['nl_b'];
				$xml .= $GLOBALS['nl_b'];
			}
			$xml .= "]>".$GLOBALS['nl_b'].$GLOBALS['nl_b'].$GLOBALS['nl_b'];
*/
			$xml .= "<list referer=\"fx-project\">".$GLOBALS['nl_b'];
			$wtab=array();
//fxDebug($felder, '$felder');
			foreach($pdat as $pid => $data)
			{
				$eaev=get_xml_element($felder, $data, $main);
				if(fxIsArray($eaev))
				{
					$xml .= "	<".$main.$eaev['ea'].">".$GLOBALS['nl_b'];
					$xml .= $eaev['ev'];
					if(fxIsArray($adat[$pid]))
					{
						foreach($adat[$pid] as $adr)
						{
							$tn='adressen';
							$eaev=get_xml_element($felder, $adr, $tn, "			");
							if(fxIsArray($eaev))
							{
								$xml .= "		<".$tn.$eaev['ea'].">".$GLOBALS['nl_b'];
								$xml .= $eaev['ev'];
								$xml .= "		</".$tn.">".$GLOBALS['nl_b'];
							}
						}
					}
					if(fxIsArray($kdat[$pid]))
					{
						foreach($kdat[$pid] as $kom)
						{
							$tn='kommunikation';
							$eaev=get_xml_element($felder, $kom, $tn, "			");
							if(fxIsArray($eaev))
							{
								$xml .= "		<".$tn.$eaev['ea'].">".$GLOBALS['nl_b'];
								$xml .= $eaev['ev'];
								$xml .= "		</".$tn.">".$GLOBALS['nl_b'];
							}
						}
					}
					$xml .= "	</".$main.">".$GLOBALS['nl_b'];
				}
			}
			$xml .= "</list>";

			// XML-Datei speichern
			fxSave($fp, $xml);
		}

		// OK. Erfolgsmeldung und Datei zum Download anbieten
		$download=false;
		if(file_exists($fp))
		{
			$meldung=str_replace('ISO-8859-1', FXP_CHARSET, get_message('Export_OK'));
			$download=true;
		}
		else
			$meldung=get_message('Export_Fehler');

		$inhalt .= '<table width="'.($mwidth-20).'" border="0" cellpadding="0" cellspacing="0">'.$nl;
		$inhalt .= '	<tr>'.$nl;
		$inhalt .= '		<td nowrap width="100%" valign="top">'.$meldung.'<br /><br /><br /></td>'.$nl;
		$inhalt .= '	</tr>'.$nl;
		if($download)
		{
			$li=fieldLinkImage('icon_download', 'im_dl', 'loader.php&url=download.inc&tp=xml&fn='.$fp, 'Download: &quot;<b>'.$fn.'</b>&quot;');
			$GLOBALS['fxpvars']['ibuffer'][]=$li;
		}
		$inhalt .= '</table>'.$nl;
	break;
}


// Buttons ausblenden
$maxstep=sizeof($steps);
if($_step >= $maxstep-1)
	$Button_Next_nv=true;
if($_step != $maxstep-1)
	$Button_XML_nv=true;
if($_step != $maxstep)
	$Button_Start_nv=true;
if(($_step == 1) || (($_step == $maxstep) && $download))
	$Button_Back_nv=true;

// Hidden
$hv=array('_step');
if($rstep != 1)
	$hv[]='_group';
if($rstep != 2)
{
	$hv[]='_ptyp';
	$hv[]='_padd';
}
foreach($hv as $h)
	fieldHidden($h, ${$h});

// Überschrift
$maskenfeldwerte['ueberschrift']='~<b>'.$txt_schritt.' '.$_step.'/'.$maxstep.':</b> <b class="grey">'.$steps[$_step].'</b><hr class="fxhr" style="width:'.($mwidth-20).'px;"><br />';

// Inhalt anzeigen
$maskenfeldwerte['textanzeige']='~'.$inhalt;
?>