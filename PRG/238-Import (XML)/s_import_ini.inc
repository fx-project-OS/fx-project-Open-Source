<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : s_import_ini.inc                                             //
// Version     : 21.1                                                         //
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
 * PF 238: Import (XML)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

// We need our XML function collection
$req_fn=fxf_fn_reqFilename('s_exim_tools', 237);
if(strlen($req_fn))
	require($req_fn);

$GLOBALS['_masknumber']=477;
setForm(true);

// Standard-Maskenbreite
$mwidth=(int)db_value("SELECT breite FROM masken WHERE nummer=477 AND mandanten_id=0");
if(!$mwidth)
	$mwidth=940;

$unknown=meldung(1449,true);	// [Unbekannt]
if($GLOBALS['fxpglobals']['lang'] == 1)
{
	$ttnew='<b>NEU</b>=Diesen Datensatz anlegen';
	$ttchg='<b>AENDERN</b>=Bestehenden Datensatz abändern';
}
else
{
	$ttnew='<b>NEW</b>=Create this dataset';
	$ttchg='<b>CHANGE</b>=Change existing dataset';
}

// Elter-Zuordnung
$etypes=array(620=>'276', 841=>'1372', 2090=>'274', 2092=>'2091', 275=>'274,275,840', 840=>'274,275,840');

// Default-Werte
$_step		= 1;
$_group		= 0;
$_xmlfile	= '';
$_xmltype	= '';
$_xmlsize	= 0;
$_xmlpath	= '';
$_xmlelem	= '';
$_dublette	= '';
$_pers_n	= array();
$_pers_c	= array();

// POST-VARS
if(!fxIsArray($_POST) || !$_POST['_step'] || (fxIsArray($_GET) && ($_GET['tr'] == $GLOBALS['fxpglobals']['tr'])))
	$_step=1;
else
	$_step=(int)$_POST['_step'];

if(($GLOBALS['fxpglobals']['tr'] == 238) || ($GLOBALS['fxpglobals']['tr'] == 240))
	$rstep=$_step+1;
else
	$rstep=$_step;

if(($GLOBALS['fxpglobals']['tr'] == 237) || ($GLOBALS['fxpglobals']['tr'] == 238))
	$_group=574;	// Personen
else if(($GLOBALS['fxpglobals']['tr'] == 239) || ($GLOBALS['fxpglobals']['tr'] == 240))
	$_group=575;	// Projekte
else if($_POST['_group'])
	$_group=(int)$_POST['_group'];

// Dokumentenupload leeren?
if(fxIsArray($_GET) && $_GET['nid'])
{
	$_POST['dokument']=array();
	$_xmlfile='';
	$_xmltype='';
	$_xmlsize=0;
	$_xmlpath='';
}
else
{
	if(isset($_POST['_xmlfile']))
		$_xmlfile=$_POST['_xmlfile'];
	if(isset($_POST['_xmltype']))
		$_xmltype=$_POST['_xmltype'];
	if(isset($_POST['_xmlsize']))
		$_xmlsize=$_POST['_xmlsize'];
	if(isset($_POST['_xmlpath']))
		$_xmlpath=$_POST['_xmlpath'];
}

$uf_new='';
$af_new='';
if(strlen($_xmlpath))
{
	$uf_new=$GLOBALS['tmppath'].$_xmlpath;
	if(!file_exists($uf_new))
		$uf_new='';
	else
	{
		$af_new=substr($uf_new, 0, -5).'a.xml';
		if(!file_exists($af_new))
			$af_new='';
		else
			require($af_new);
	}
}

if(isset($_POST['_xmlelem']))
	$_xmlelem=$_POST['_xmlelem'];
if(isset($_POST['_dublette']))
	$_dublette=$_POST['_dublette'];
if(isset($_POST['_pers_n']))
	$_pers_n=$_POST['_pers_n'];
if(isset($_POST['_pers_c']))
	$_pers_c=$_POST['_pers_c'];

//echo('<B>$_step:</B> '.$_step.', <B>$rstep:</B> '.$rstep.'<br />');

// Datei uploaden
if(($rstep == 2) && $_POST['Button_Next'])
{
	// Info des Upload-File ermitteln
	if(fxIsArray($_POST['dokument']))
	{
		$uf_name=$_POST['dokument']['name'];
		$uf_type=$_POST['dokument']['type'];
		$uf_tmp=$_POST['dokument']['new_name'];
		$uf_error=$_POST['dokument']['error'];
		$uf_size=$_POST['dokument']['size'];
	}
	else
	{
		$uf_name='';
		$uf_type='';
		$uf_tmp='';
		$uf_error=0;
		$uf_size=0;
	}
	if(!$uf_error && (!strlen($uf_new) || strlen($uf_name)))
	{
		// Prüfen, ob Datei ausgewählt wurde.
		if(!$uf_error && !strlen($uf_name))
		{
			error_msg(925, array('textanzeige'), 'XML');	// Im Feld "$var1" muss ein gültiger Dateiname ausgewählt bzw. eingetragen werden!
			$uf_error=66;
			// ...Fehler als Kopie anlegen für dargestelltes Feld
			if(fxIsArray($GLOBALS['fxpvars']['lookups']) && fxIsArray($GLOBALS['fxpvars']['lookups'][$GLOBALS['_masknumber']]))
				$GLOBALS['fxpvars']['lookups'][$GLOBALS['_masknumber']]['dokument']=$GLOBALS['fxpvars']['lookups'][$GLOBALS['_masknumber']]['textanzeige'];
		}

		// Prüfen, ob gültige Textdatei
		if(!$uf_error && ($uf_type != 'text/xml'))
		{
			error_msg(1056, array('textanzeige'), $uf_name);	// Die Datei $var1 ist keine gültige XML-Datei!
			$uf_error=66;
			// ...Fehler als Kopie anlegen für dargestelltes Feld
			if(fxIsArray($GLOBALS['fxpvars']['lookups']) && fxIsArray($GLOBALS['fxpvars']['lookups'][$GLOBALS['_masknumber']]))
				$GLOBALS['fxpvars']['lookups'][$GLOBALS['_masknumber']]['dokument']=$GLOBALS['fxpvars']['lookups'][$GLOBALS['_masknumber']]['textanzeige'];
		}

		// Datei kopieren in den TMP-Ordner
		if(fxIsArray($_POST['dokument']) && isset($_POST['dokument']['error']) && !$uf_error)
		{
			$_xmlpath='~'.fx_date('YmdHis').$GLOBALS['fxpglobals']['person'].'x.xml';
			$uf_new=$GLOBALS['tmppath'].$_xmlpath;
			$result=@copy($uf_tmp, $uf_new);
			if($result !== true)	// FEHLER
			{
				error_msg(1605, array('textanzeige'), $uf_tmp);	// Die Datei "$var1" konnte zur Bearbeitung nicht in die temporäre Zwischenablage kopiert werden!
				$uf_error=66;
				// ...Fehler als Kopie anlegen für dargestelltes Feld
				if(fxIsArray($GLOBALS['fxpvars']['lookups']) && fxIsArray($GLOBALS['fxpvars']['lookups'][$GLOBALS['_masknumber']]))
					$GLOBALS['fxpvars']['lookups'][$GLOBALS['_masknumber']]['dokument']=$GLOBALS['fxpvars']['lookups'][$GLOBALS['_masknumber']]['textanzeige'];
			}
			else
			{
				$_xmlfile=$uf_name;
				$_xmlsize=$uf_size;
				$_xmltype=$uf_type;
			}
		}
	}
	else if($uf_new)
	{
		if(file_exists($uf_new))
			$uf_error=0;
		else
		{
			error_msg(925, array('textanzeige'), 'XML');	// Im Feld "$var1" muss ein gültiger Dateiname ausgewählt bzw. eingetragen werden!
			$uf_error=66;
			// ...Fehler als Kopie anlegen für dargestelltes Feld
			if(fxIsArray($GLOBALS['fxpvars']['lookups']) && fxIsArray($GLOBALS['fxpvars']['lookups'][$GLOBALS['_masknumber']]))
				$GLOBALS['fxpvars']['lookups'][$GLOBALS['_masknumber']]['dokument']=$GLOBALS['fxpvars']['lookups'][$GLOBALS['_masknumber']]['textanzeige'];
		}
	}
	else
		$uf_error=99;

	if($uf_error)
	{
		$err=true;
		if($uf_error != 66)
		{
			error_msg(1607, array('textanzeige'), $uf_error, $uf_name);	// Fehler #$var1 beim uploaden der Datei "$var2"!
			// ...Fehler als Kopie anlegen für dargestelltes Feld
			if(fxIsArray($GLOBALS['fxpvars']['lookups']) && fxIsArray($GLOBALS['fxpvars']['lookups'][$GLOBALS['_masknumber']]))
				$GLOBALS['fxpvars']['lookups'][$GLOBALS['_masknumber']]['dokument']=$GLOBALS['fxpvars']['lookups'][$GLOBALS['_masknumber']]['textanzeige'];
		}
	}
	else if(file_exists($uf_new))
	{
		$af_new=substr($uf_new, 0, -5).'a.xml';
		if(file_exists($af_new))
			require($af_new);
		else
		{
			// XML-Datei laden und parsen
			$data='';
			$fp=fopen($uf_new, 'r');
			if($fp)
			{
				$data=fread($fp, filesize($uf_new));
				fclose($fp);
			}
			if(strlen($data))
			{
//echo('<b>XML-Datei:</b><hr size=1 /><pre>'.fxHtmlEncode($data).'</pre><hr size=1 />');
			    $parser=xml_parser_create(FXP_CHARSET);
			    xml_parser_set_option($parser, XML_OPTION_CASE_FOLDING, 0);
			    xml_parser_set_option($parser, XML_OPTION_SKIP_WHITE, 1);
				xml_parser_set_option($parser, XML_OPTION_TARGET_ENCODING, FXP_CHARSET);
			    xml_parse_into_struct($parser, $data, $values, $xtgs);
			    xml_parser_free($parser);
//fxDebug($xtgs,'$xtgs', 0);
				if(fxIsArray($values))
				{
//fxDebug($values,'$values', 0);
					$st=array('&amp;'=>'&', '&quot;'=>'"', '&#039;'=>'´', '&lt;'=>'<', '&gt;'=>'>', "'"=>'´');

					$lst=array();
					$lvt=array(0=>'$lst');

					$qual=array();
					$elem=array();

					$ltx='// XML-ARRAY'.$GLOBALS['nl_b'].'$lst=array();'.$GLOBALS['nl_b'];
					$lvl=0;
					foreach($values as $ln => $val)
					{
						if($val['level'] < 2)
							continue;
						$xtg=strtolower($val['tag']);
						if($val['type'] == 'open')
						{
							$lvl++;
							$lvt[$lvl]=$lvt[$lvl-1].'[\''.$xtg.'\'][\''.$ln.'\']';
							if(!isset($qual[$xtg]))
								$ltx .= $lvt[$lvl-1].'[\''.$xtg.'\']=array();'.$GLOBALS['nl_b'];
							$qual[$xtg]=array();
							if(!isset($qual[$xtg][$ln]))
								$ltx .= $lvt[$lvl].'=array();'.$GLOBALS['nl_b'];
							$qual[$xtg][$ln]=true;
							if(fxIsArray($val['attributes']))
							{
								foreach($val['attributes'] as $qxtg => $value)
								{
									$qxtg=strtolower($qxtg);
									$elem[$qxtg]=true;
									$ltx .= $lvt[$lvl].'[\''.$qxtg.'\']=\''.strtr($value, $st).'\';'.$GLOBALS['nl_b'];
								}
							}
						}
						else if($val['type'] == 'close')
						{
							$lvl--;
						}
						else if($val['type'] == 'complete')
						{
							$elem[$xtg]=true;
							$ltx .= $lvt[$lvl].'[\''.$xtg.'\']=\''.strtr($val['value'], $st).'\';'.$GLOBALS['nl_b'];
						}
					}

					// geparste Datei schreiben
					$af_cnt='<'.'?'.'php'.$GLOBALS['nl_b'];
					if(sizeof($qual))
					{
						$af_cnt .= '// QUALIFIER'.$GLOBALS['nl_b'].'$qual=array ('.$GLOBALS['nl_b'];
						foreach($qual as $qn => $dmy)
							$af_cnt .= "	'$qn' => true,".$GLOBALS['nl_b'];
						$af_cnt .= ');'.$GLOBALS['nl_b'].$GLOBALS['nl_b'];
					}
					if(sizeof($elem))
					{
						$af_cnt .= '// ELEMENTS'.$GLOBALS['nl_b'].'$elem=array ('.$GLOBALS['nl_b'];
						foreach($elem as $en => $dmy)
							$af_cnt .= "	'$en' => true,".$GLOBALS['nl_b'];
						$af_cnt .= ');'.$GLOBALS['nl_b'].$GLOBALS['nl_b'];
					}
					$af_cnt .= $ltx.'?'.'>';
//echo('<b>$ltx:</b><hr size=1 /><pre>'.fxHtmlEncode($ltx).'</pre><hr size=1 />');
					$fe=fxSave($af_new, $af_cnt);
					if(!$fe)
						require($af_new);
				}
			}
		}
	}
}

if($_POST['Button_Import'] && !fxIsArray($_pers_n) && !fxIsArray($_pers_c))
{
	error_msg(1642, false);	// Es wurden keine Einträge angehakt.
	$err=true;
}

// Nächsten Schritt ermitteln...
if(!$err)
{
	if($_POST['Button_Next'] || $_POST['Button_Import'])
		$_step++;
	if($_POST['Button_Back'])
		$_step--;
	if($_POST['Button_Start'])
		$_step=1;
}

if(($GLOBALS['fxpglobals']['tr'] == 238) || ($GLOBALS['fxpglobals']['tr'] == 240))
	$rstep=$_step+1;
else
	$rstep=$_step;

// Texte
$txt_schritt=meldung(573, false);

// Import-Schritte einladen
if($GLOBALS['fxpglobals']['tr'] != 236)
	$startid='';
else
	$startid='568,';
$steps=array();
$sql="SELECT tabwert FROM wertetab WHERE mandanten_id=0 AND id_feld=255 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND wertetabid IN (".$startid."590,591,592,593) ORDER BY position";
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

	case 2:	// XML-Quelldatei auswählen
		$a=explode('|', str_replace('ISO-8859-1', FXP_CHARSET, meldung(3274,false)));	// ISO-8859-1 XML|Keine Datei vorhanden!|Hier klicken zum Uploaden einer neuen XML-Datei
		$dokument_htext=FXP_CHARSET.' XML';		// -> XML
		$dokument_fpn=$a[1];					// -> Keine Datei vorhanden!
		$dokument_utt=$a[2];					// -> Hier klicken zum Uploaden einer neuen XML-Datei

		$inhalt .= '<table width="'.($mwidth-20).'" border="0" cellpadding="0" cellspacing="0">'.$nl;
		if($_xmlfile)
		{
			$uf_new=$GLOBALS['tmppath'].$_xmlpath;
			if(file_exists($uf_new))
			{
				$_POST['dokument']['name']=$_xmlfile;
				$_POST['dokument']['type']=$_xmltype;
				$_POST['dokument']['size']=$_xmlsize;
			}
		}
		$inhalt .= '	<tr>'.$nl;
//function fieldFileDoc($fieldname, $value='', $width=0, $height=0, $multi=0, $tooltip='', $jsfunc='', $form='')
		$inhalt .= '		<td nowrap valign="top">'.fieldFileDoc('dokument',$_POST['dokument'],($mwidth-22)).'</td>'.$nl;
		$inhalt .= '	</tr>'.$nl;
		$inhalt .= '</table>'.$nl;
	break;

	case 3:	// XML-Felder zuordnen
		if(fxIsArray($elem))
			ksort($elem);
		$table=array('personen', 'adressen', 'kommunikation');

		$felder=load_exim_cfg($_group);
		if(fxIsArray($felder))
		{
			$z=0;

			// Abfrage Doubletten
			$dbl_arr[1]=meldung(1746, true);	// Bei Dubletten Datensätze <B>NEU</B> anlegen?

			$inhalt .= '<table width="'.($mwidth-20).'" border="0" cellpadding="0" cellspacing="0">'.$nl;
			$inhalt .= '	<tr>'.$nl;
			$inhalt .= '		<td valign="top">'.sys_check('_dublette', $dbl_arr, false).'<br /></td>'.$nl;
			$inhalt .= '	</tr>'.$nl;
			$inhalt .= '</table>'.$nl;

			// XML-Elemente
			$sel[0]='('.meldung(597, false).')';	// Ohne Zuordnung
			if(fxIsArray($elem))
			{
				foreach($elem as $en => $dmy)
					$sel[$en]='['.$en.']';
			}
//fxDebug($sel, $sel);
			$sname='_xmlelem';
//fxDebug($felder, 'Felder');

			$inhalt .= '<table width="'.($mwidth-20).'" border="0" cellpadding="0" cellspacing="0">'.$nl;

			// Überschrift
			$inhalt .= '	<tr>'.$nl;
			$inhalt .= '		<td valign="top">&nbsp;</td>'.$nl;
			$inhalt .= '		<td class="rhl" valign="top" style="'.$xmlths.'">FXP Element Name</td>'.$nl;
			$inhalt .= '		<td class="rhl" valign="top" style="'.$xmlths.'">XML Element Name</td>'.$nl;
			$inhalt .= '	</tr>'.$nl;

			foreach($table as $tabname)
			{
				if($z)
				{
					$inhalt .= '	<tr>'.$nl;
					$inhalt .= '		<td>&nbsp;</td>'.$nl;
					$inhalt .= '		<td colspan="2" style="'.$xmlels.'">&nbsp;</td>'.$nl;
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

					// Selected value
					$sv=0;
					$fl=strtolower($feld);
					$xl=strtolower($farr['xml']);
					foreach($sel as $sval => $dummy)
					{
						if(($sval == $fl) || ($sval == $xl))
							$sv=$sval;
						else if($farr['alias'])
						{
							$alias=explode('|', $farr['alias']);
							foreach($alias as $al)
							{
								$al=strtolower(trim($al));
								if($sval == $al)
								{
									$sv=$sval;
									break;
								}
							}
						}

						if(strlen($sv) && ($sv != 0))
							break;
					}

					$inhalt .= '	<tr>'.$nl;
					$inhalt .= '		<td nowrap><img src="'.$GLOBALS['gfxpath'].$img.'.png">'.fieldHidden('_xmlid['.$z.']',$farr['id']).'&nbsp;</td>'.$nl;
					$inhalt .= '		<td'.$cls.' width="100%" nowrap style="padding:1px;paddin-right:0;border-right:1px solid #'.$GLOBALS['msk_nbcol'].';"><span id="dummy" class="'.$lcls.'" tooltip="'.$farr['text'].'" style="cursor:help;">&nbsp;'.$farr['name'].'&nbsp;</span></td>'.$nl;
					$GLOBALS['_xmlelem'.$feld.'_ds']=0;
					$GLOBALS['_xmlelem'.$feld.'_ds_array']=$sel;
					$inhalt .= '		<td'.$cls.' nowrap style="padding:1px;paddin-right:0;border-right:1px solid #'.$GLOBALS['msk_nbcol'].';">'.fieldSelect($sname.'['.$feld.']',$sv,$mwidth/2).'</td>'.$nl;
					$inhalt .= '	</tr>'.$nl;

					$z++;
				}
			}
			if($z)
			{
				$inhalt .= '	<tr>'.$nl;
				$inhalt .= '		<td>&nbsp;</td>'.$nl;
				$inhalt .= '		<td colspan="2" style="'.$xmlels.'">&nbsp;</td>'.$nl;
				$inhalt .= '	</tr>'.$nl;
			}
			$inhalt .= '</table>'.$nl;
		}
	break;

	case 4:	// Zu importierende Datensätze auswählen
		if(!fxIsArray($lst))
		{
			$inhalt .= '<b class="red">'.meldung(896, false).'</b>'.$nl;	// Keine Einträge in der XML-Liste gefunden!
			break;
		}
		$felder=load_exim_cfg($_group);
//fxDebug($felder, 'Felder');
		if(!fxIsArray($felder))
		{
			$inhalt .= '<b class="red">'.meldung(897, false).'</b>'.$nl;	// Keine Einträge zu den Felddefinitionen gefunden!
			break;
		}

		// Alle vorhandenen Nachschlagewerte ermitteln
		$nw=import_getLookupValues($felder);
//fxDebug($nw, '$nw: Nachschlagewerte');

		// Duplikats-Liste löschen
		$GLOBALS['dupl_imp']=array();

		// Duplicats-Meldungen
		$dupl_m0=meldung(3026,true);	// Keine Duplette, da in der Datenbank kein Datensatz mit dem gleichen Namen gefunden wurde
		$dupl_m1=meldung(1747,true);	// Duplette, da Primärschlüssel (= ID) bereits existiert
		$dupl_m2=meldung(1748,true);	// Duplette, da in der Datenbank ein Datensatz mit gleichem Namen bereits existiert
		$dupl_m3=meldung(1749,true);	// Duplette, da in der Import-Datei ein Datensatz mit gleichem Namen bereits definiert wurde

		$z=0;
		$inhalt .= '<table width="'.($mwidth-20).'" border="0" cellpadding="0" cellspacing="0">'.$nl;

		// Überschrift
		if($GLOBALS['fxpglobals']['lang'] == 1)
		{
			$lnew='<b>NEU</b>=Neuen Datensatz anlegen<br /><i>(Checkbox anhaken um diesen Datensatz anzulegen)</i>';
			$lchg='<b>AENDERN</b>=Bestehenden Datensatz abändern<br /><i>(Checkbox anhaken um den bestehenden Datensatz mit diesen Daten abzuändern)</i>';
			$ladd='AAnzahl der Adress-Datensätze';
			$lcom='KAnzahl der Kommunikations-Datensätze';
		}
		else
		{
			$lnew='<b>NEW</b>=Create new dataset<br /><i>(Tick checkbox to create this dataset)</i>';
			$lchg='<b>CHANGE</b>=Change existing dataset<br /><i>(Tick checkbox to change the existing dataset with this data)</i>';
			$ladd='ANumber of address datasets';
			$lcom='CNumber of communication datasets';
		}
		$inhalt .= '	<tr>'.$nl;
		$inhalt .= '		<td class="rhl" nowrap valign="top" tooltip="'.$lnew.'" style="'.$xmlths.'cursor:help;">+<img src="'.$GLOBALS['gfxpath'].'tts.png" align="top"></td>'.$nl;
		$inhalt .= '		<td class="rhl" nowrap valign="top" tooltip="'.$lchg.'" style="'.$xmlths.'cursor:help;">?<img src="'.$GLOBALS['gfxpath'].'tts.png" align="top"></td>'.$nl;
		$inhalt .= '		<td class="rhl" nowrap valign="top" style="'.$xmlths.'">St.</td>'.$nl;
		$inhalt .= '		<td class="rhl" nowrap valign="top" style="'.$xmlths.'">'.ucfirst($_xmlelem['pname']);
		if($_xmlelem['vorname'])
			$inhalt .= ', '.ucfirst($_xmlelem['vorname']);
		$inhalt .= '</td>'.$nl;
		$inhalt .= '		<td class="rhl" nowrap valign="top" style="'.$xmlths.'">'.ucfirst($_xmlelem['personentyp']).'</td>'.$nl;
		$inhalt .= '		<td class="rhl" nowrap valign="top" style="'.$xmlths.'">'.ucfirst($_xmlelem['elter']).'</td>'.$nl;
		$inhalt .= '		<td class="rhl" nowrap valign="top" tooltip="'.substr($ladd,1).'" style="'.$xmlths.'cursor:help;">#'.substr($ladd,0,1).'<img src="'.$GLOBALS['gfxpath'].'tts.png" align="top"></td>'.$nl;
		$inhalt .= '		<td class="rhl" nowrap valign="top" tooltip="'.substr($lcom,1).'" style="'.$xmlths.'cursor:help;">#'.substr($lcom,0,1).'<img src="'.$GLOBALS['gfxpath'].'tts.png" align="top"></td>'.$nl;
		$inhalt .= '	</tr>'.$nl;

		$pc=0;
		$la=8;
		foreach($lst['personen'] as $pid => $p)
		{
			$la=8;

			// Klasse
			$cls=' class="rv'.(($pc%2)+1).'" id="ptd'.$pc.'"';

			// Ist der Datensatz eine Dublette?
			$dublicate_id='';
			$ra=import_argumente('personen', $p, $nw, $felder, $_xmlelem);
			if(fxIsArray($ra))
				$dubl_status=check_dublicate('personen', $ra);
			else
				$dubl_status=-1;

			// Name ermitteln
			$name=trim($p[$_xmlelem['pname']]);
			$vname=trim($p[$_xmlelem['vorname']]);
			if(strlen($vname))
				$name .= ', '.$vname;

			$inhalt .= '	<tr>'.$nl;

			// Checkbox (Neuanlage)
			if($dubl_status < 0)
				$inhalt .= '		<td'.$cls.' valign="top" style="'.$xmltds.'">&nbsp;</td>'.$nl;
			else
			{
				$checked='';
				if(($_POST['Button_Back'] || $err) && $_pers_n[$pid])
					$checked='1';
				else if(!$dubl_status || (($dubl_status > 0) && $_dublette))
					$checked='1';
				else if($name == '?')
					$checked='1';
				$inhalt .= '		<td'.$cls.' valign="top" align="center" style="'.$xmltds.'">'.fieldCheckbox('_pers_n['.$pid.']','',$checked,false,false,false,$GLOBALS['ttnew']).'</td>'.$nl;
			}

			// Checkbox (Ändern)
			if($dubl_status < 1)
				$inhalt .= '		<td'.$cls.' valign="top" style="'.$xmltds.'">&nbsp;</td>'.$nl;
			else
			{
				$checked='';
				if(($_POST['Button_Back'] || $err) && $_pers_c[$pid])
					$checked='1';
				$inhalt .= '		<td'.$cls.' valign="top" align="center" style="'.$xmltds.'">'.fieldCheckbox('_pers_c['.$pid.']','',$checked,false,false,false,$GLOBALS['ttchg']).'</td>'.$nl;
			}

			// Dublette
			if($dubl_status < 1)
				$dimg1='green';
			else
				$dimg1='grey';
			if($dubl_status == 1)
			{
				$dimg2='c_priv';
				$ttl=$dupl_m1;
			}
			else if($dubl_status == 2)
			{
				$dimg2='c_rept';
				$ttl=$dupl_m2;
			}
			else if($dubl_status == 3)
			{
				$dimg2='c_todo';
				$ttl=$dupl_m3;
			}
			else
			{
				$dimg2='';
				$ttl=$dupl_m0;
			}
			if(strlen($dimg2))
				$dimg2='<img id="dimg2" src="'.$GLOBALS['gfxpath'].$dimg2.'.png" align="top" style="padding:2px;">';
			$inhalt .= '		<td'.$cls.' valign="top" nowrap tooltip="'.$ttl.'" style="'.$xmltds.'cursor:help;"><img src="'.$GLOBALS['gfxpath'].'tl_'.$dimg1.'.png" align="top" style="padding:4px 2px;">'.$dimg2.'</td>'.$nl;

			// Name
			$tdtt	= ' style="'.$xmltds.'"';
			$tt		= '';
			if($dublicate_id)
			{
				$pdt=array();

				$sql  = "SELECT personen_id, pname, vorname, geschlecht, elter FROM personen";
				$sql .= " WHERE personen_id IN (".$dublicate_id.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$sql .= " ORDER BY pname, vorname";
				$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
				if(fxIsArray($tmp))
				{
					foreach($tmp as $t)
					{
						$pname=trim($t['pname']);
						$vname=trim($t['vorname']);
						$name=$pname;
						if(strlen($vname))
							$name .= ', '.$vname;
						$pdt[$t['personen_id']]=array('name'=>$name, 'pname'=>$pname, 'vname'=>$vname, 'sex'=>$t['geschlecht'], 'elter'=>$t['elter'], 'ename'=>"");

						if($t['elter'])
						{
							$sql="SELECT pname, vorname FROM personen WHERE personen_id=".$t['elter']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
							$tmp=db_values($sql);
//fxDebug($tmp,$sql);
							if(fxIsArray($tmp))
							{
								$epname=trim($tmp['pname']);
								$evname=trim($tmp['vorname']);
								$ename=$epname;
								if(strlen($evname))
									$ename .= ', '.$evname;
								$pdt[$t['personen_id']]['ename']=$ename;
							}
						}
					}

					$sql  = "SELECT a.personen_id, t.tabwert AS typ, a.strasse, l.tabwert AS lkz, a.plz, a.ort FROM adressen a, wertetab t, wertetab l";
					$sql .= " WHERE a.personen_id IN (".$dublicate_id.") AND a.mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$sql .= " AND (t.wertetabid=a.adresstyp AND t.id_sprache=".$GLOBALS['fxpglobals']['lang']." AND t.mandanten_id IN (0,".$GLOBALS['fxpglobals']['client']."))";
					$sql .= " AND (l.wertetabid=a.landkz AND l.id_sprache=".$GLOBALS['fxpglobals']['lang']." AND l.mandanten_id IN (0,".$GLOBALS['fxpglobals']['client']."))";
					$sql .= " ORDER BY t.tabwert, a.strasse";
					$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
					if(fxIsArray($tmp))
					{
						foreach($tmp as $t)
						{
							if($t['strasse'])
								$padr=$t['strasse'].',&nbsp;';
							else
								$padr='';
							$padr .= $t['lkz'].'-'.$t['plz'].'&nbsp;'.$t['ort'];
							$pdt[$t['personen_id']]['adr'][$t['typ']][]=$padr;
						}
					}

					$sql  = "SELECT k.personen_id, t.tabwert AS typ, k.kommunikation_txt FROM kommunikation k, wertetab t";
					$sql .= " WHERE k.personen_id IN (".$dublicate_id.") AND k.mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$sql .= " AND (t.wertetabid=k.kommunikationstyp AND t.id_sprache=".$GLOBALS['fxpglobals']['lang']." AND t.mandanten_id IN (0,".$GLOBALS['fxpglobals']['client']."))";
					$sql .= " ORDER BY t.tabwert, k.kommunikation_txt";
					$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
					if(fxIsArray($tmp))
					{
						foreach($tmp as $t)
							$pdt[$t['personen_id']]['kom'][$t['typ']][]=$t['kommunikation_txt'];
					}
//fxDebug($pdt, '$pdt');

					// Tooltip anzeigen
					$tbl='<table border=0 cellpadding=0 cellspacing=0>';
					if($dubl_status == 1)
						$tbcol='green';
					else
						$tbcol='red';
					foreach($pdt as $dat)
					{
						$br='';
						if(fxIsArray($dat['adr']) || fxIsArray($dat['kom']))
							$br='<br /><br />';
						$pelt='';
						if(strlen($dat['ename']))
							$pelt='&nbsp;&nbsp;<i class=grey>(&larr;&nbsp;'.$dat['ename'].')</i>';
						$tbl .= '<tr><td colspan=2>&rarr;&nbsp;&nbsp;<b class='.$tbcol.'>'.$dat['name'].'</b>'.$pelt.$br.'</td></tr>';
						if(fxIsArray($dat['adr']))
						{
							foreach($dat['adr'] as $atyp => $uadr)
							{
								$tbl .= '<tr>';
								$tbl .= '<tr><td valign=top><span class=lit>'.$atyp.':&nbsp;</td>';
								$tbl .= '<td>';
								foreach($uadr as $u)
									$tbl .= text_wrap($u,true,64).'<br />';
								$tbl .= '</td>';
								$tbl .= '</tr>';
							}
						}
						if(fxIsArray($dat['kom']))
						{
							if(fxIsArray($dat['adr']))
								$tbl .= '<tr><td colspan=2>&nbsp;</td></tr>';
							foreach($dat['kom'] as $ktyp => $ukom)
							{
								$tbl .= '<tr>';
								$tbl .= '<tr><td valign=top><span class=lit>'.$ktyp.':&nbsp;</td>';
								$tbl .= '<td>';
								foreach($ukom as $u)
									$tbl .= text_wrap($u,true,64).'<br />';
								$tbl .= '</td>';
								$tbl .= '</tr>';
							}
						}
					}
					$tbl .= '</table>';
					$tdtt=' tooltip="'.$tbl.'" style="'.$xmltds.'cursor:help;"';
				}
			}
			$inhalt .= '		<td'.$cls.' valign="top" width="100%" nowrap'.$tdtt.'><b>&nbsp;'.$name.$tt.'&nbsp;</b></td>'.$nl;

			// Personentyp
			$ptyp=trim($p[$_xmlelem['personentyp']]);
			$inhalt .= '		<td'.$cls.' valign="top" nowrap style="'.$xmltds.'">&nbsp;<i>'.$ptyp.'</i>&nbsp;</td>'.$nl;

			// Elter
			$elt=trim($p[$_xmlelem['elter']]);
			$inhalt .= '		<td'.$cls.' valign="top" style="'.$xmltds.'">&nbsp;<i>'.$elt.'</i>&nbsp;</td>'.$nl;

			// Adressen
			$adr=0;
			if(fxIsArray($p['adressen']))
				$adr=sizeof($p['adressen']);
			$inhalt .= '		<td'.$cls.' valign="top" align="right" style="'.$xmltds.'">&nbsp;'.$adr.'&nbsp;</td>'.$nl;

			// Kommunikation
			$kom=0;
			if(fxIsArray($p['kommunikation']))
				$kom=sizeof($p['kommunikation']);
			$inhalt .= '		<td'.$cls.' valign="top" align="right" style="'.$xmltds.'">&nbsp;'.$kom.'&nbsp;</td>'.$nl;

			$inhalt .= '	</tr>'.$nl;

			$z++;

			// Adressen + Kommunikation in neuer Zeile
			if($adr || $kom)
			{
				$la=5;
				$ak='<div style="'.$xmlels.$xmltds.'box-shadow:3px 3px 6px rgba(0,0,0, 0.25) inset;padding:4px;padding-bottom:16px;">';
				if(fxIsArray($p['adressen']))
				{
					foreach($p['adressen'] as $adat)
					{
						$txt=text_wrap($adat[strtolower($felder['strasse']['xml'])].', '.$adat[strtolower($felder['landkz']['xml'])].'-'.$adat[strtolower($felder['plz']['xml'])].' '.$adat[strtolower($felder['ort']['xml'])],true,36);
						$ak .= '<div class="lightergrey" style="display:inline-block;width:200px;">'.$adat[strtolower($felder['adresstyp']['xml'])].':</div>&nbsp;<span class="darkgrey">'.$txt.'</span><br />';
					}
				}
				if(fxIsArray($p['kommunikation']))
				{
					if(fxIsArray($p['adressen']))
						$ak .= '<br />';
					foreach($p['kommunikation'] as $kdat)
					{
						$txt=text_wrap($kdat[strtolower($felder['kommunikation_txt']['xml'])],true,36);
						$ak .= '<div class="lightergrey" style="display:inline-block;width:200px;">'.$kdat[strtolower($felder['kommunikationstyp']['xml'])].':</div>&nbsp;<span class="darkgrey">'.$txt.'</span><br />';
					}
				}
				$ak .= '</div>';

				$inhalt .= '	<tr>'.$nl;
				$inhalt .= '		<td valign="top" colspan="3" style="'.$xmlels.'">&nbsp;</td>'.$nl;
				$inhalt .= '		<td'.$cls.' valign="top" colspan="5">'.$ak.'</td>'.$nl;
				$inhalt .= '	</tr>'.$nl;

				$z++;
			}

			$pc++;
		}
		if($z)
		{
			$inhalt .= '	<tr>'.$nl;
			if($la < 8)
				$inhalt .= '		<td colspan="'.(8-$la).'">&nbsp;</td>'.$nl;
			$inhalt .= '		<td colspan="'.$la.'" style="'.$xmlels.'">&nbsp;</td>'.$nl;
			$inhalt .= '	</tr>'.$nl;
		}
		$inhalt .= '</table>'.$nl;
	break;

	case 5:	// Ergebnis
		if(!fxIsArray($lst))
		{
			$inhalt .= '<b class="red">'.meldung(896, false).'</b>'.$nl;	// Keine Einträge in der XML-Liste gefunden!
			break;
		}
		$felder=load_exim_cfg($_group);
		if(!fxIsArray($felder))
		{
			$inhalt .= '<b class="red">'.meldung(897, false).'</b>'.$nl;	// Keine Einträge zu den Felddefinitionen gefunden!
			break;
		}
//fxDebug($felder, '$felder');
//fxDebug($_pers_n, '$_pers_n');
//fxDebug($_pers_c, '$_pers_c');
//echo('<B>$_xmlfile:</B> '.$_xmlfile.' - <B>$_xmlsize:</B> '.$_xmlsize.'<br />');
//fxDebug($lst);

		// Default Elter
		$defelt=(int)db_value("SELECT personen_id FROM personen WHERE personentyp=274 AND mandanten_id=".$GLOBALS['fxpglobals']['client']);

		// Temp. Tabelle "Import"
		$sql="SELECT id FROM import WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND dateigroesse=".$_xmlsize." AND ".db_like('dateiname', $_xmlfile);
		$tid=db_value($sql);
//fxDebug($tid, $sql);
		if($tid)
		{
			$sql="DELETE FROM import WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND dateigroesse=".$_xmlsize." AND ".db_like('dateiname', $_xmlfile);
			$err=db_x($sql);
			if($err)
			{
				$inhalt .= '<font class="bold red">'.meldung(898, false).'</font>'.$nl;	// Die Import-Datei existiert bereits in der Datenbank und konnte nicht gelöscht werden!
				break;
			}
		}

		// Alle vorhandenen Nachschlagewerte ermitteln
		$nw=import_getLookupValues($felder);
//fxDebug($nw, '$nw: Nachschlagewerte');

		// Jeden Datensatz der Reihe nach durchgehen
		$fname=convert_string($_xmlfile, 'todb');
		$apid=$GLOBALS['fxpglobals']['person'];
		$adtm=$GLOBALS['datetime'];
		$elter=array();
		$zusatz=array('adressen', 'kommunikation');
		foreach($lst['personen'] as $pid => $p)
		{
			$epid='';
			$ra=import_argumente('personen', $p, $nw, $felder, $_xmlelem);
			if(fxIsArray($ra))
			{
//fxDebug($ra, $pid);
				$mpid=id_max('import', 'id');
				$ierr=import_db('personen', $mpid, 0, $fname, $_xmlsize, $ra, $apid, $adtm, $_pers_n[$pid], $_pers_c[$pid]);
				if($ierr)
					$err=$ierr;

				// Zusatz: Adressen + Kommunikation
				foreach($zusatz as $zus)
				{
					if(fxIsArray($p[$zus]))
					{
						foreach($p[$zus] as $zusa)
						{
							$ra=import_argumente($zus, $zusa, $nw, $felder, $_xmlelem, $epid);
							if(fxIsArray($ra))
							{
//fxDebug($ra, $pid);
								$mtid=id_max('import', 'id');
								$ierr=import_db($zus, $mtid, $mpid, $fname, $_xmlsize, $ra, $apid, $adtm, $_pers_n[$pid], $_pers_c[$pid]);
								if($ierr)
									$err=$ierr;
							}
						}
					}
				}
			}
		}

		// Log-Datei ausgeben
		$sql="SELECT * FROM import WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND dateigroesse=".$_xmlsize." AND ".db_like('dateiname', $_xmlfile)." ORDER BY id";
		$ilst=db_values($sql, '*');
//fxDebug($ilst, $sql);
		if(fxIsArray($ilst))
		{
			$iws=0;	// Import with success (Zähler)
			$iwe=0;	// Import with error (Zähler)

			$st=array('&quot;'=>'"', '_+_'=>'&quot;');

			$sql="SELECT wertetabid, tabwert FROM wertetab WHERE id_feld=1605 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id=0";
			$istat=get_array($sql, 'wertetabid', 'tabwert');
//fxDebug($istat, $sql);

			$inhalt .= '<table width="'.($mwidth-20).'" border="0" cellpadding="0" cellspacing="0">'.$nl;

			// Überschrift
			$inhalt .= '	<tr>'.$nl;
			$inhalt .= '		<td class="rhl" valign="top" style="'.$xmlths.'">'.get_text(1048, '5').'</td>'.$nl;		// Nr.
			$inhalt .= '		<td class="rhl" valign="top" style="'.$xmlths.'">'.get_text(426, '5').'</td>'.$nl;		// Icon
			$inhalt .= '		<td class="rhl" valign="top" style="'.$xmlths.'">'.get_text(1563, '15').'</td>'.$nl;	// Daten
			$inhalt .= '		<td class="rhl" valign="top" style="'.$xmlths.'">'.get_text(1440, '10').'</td>'.$nl;	// Tabelle
			$inhalt .= '		<td class="rhl" valign="top" style="'.$xmlths.'">'.get_text(1605, '10').'</td>'.$nl;	// Status
			$inhalt .= '	</tr>'.$nl;

			foreach($ilst as $z => $l)
			{
				$cls=' class="rv'.(($z%2)+1).'"';

				$inhalt .= '	<tr>'.$nl;

				// Nr.
				$inhalt .= '	<td'.$cls.' align="right" nowrap style="'.$xmltds.'"><b class="grey">'.($z+1).'</b>&nbsp;</td>'.$nl;

				// Symbol
				if($l['import_status'] == 887)
					$ic='neutral';
				else if(($l['import_status'] > 887) && ($l['import_status'] < 892))
				{
					$ic='green';
					$iws++;
				}
				else
				{
					$ic='red';
					$iwe++;
				}
				$inhalt .= '<td'.$cls.' align="center" nowrap style="'.$xmltds.'"><img src="'.$GLOBALS['gfxpath'].'tl_'.$ic.'.png" align="top" style="padding:4px 2px;"></td>'.$nl;

				// Datensatz
				$imp='';
				$arg=@eval(strtr($l['argumente'], $st));

				if(fxIsArray($imp))
				{
					if(isset($imp['pname']))
					{
						if($imp['pname'] != 'NULL')
							$ds=$imp['pname'];
						if($imp['vorname'] != 'NULL')
						{
							if(strlen($ds))
								$ds .= ', ';
							$ds .= $imp['vorname'];
						}
					}
					else if(isset($imp['strasse']))
					{
						if($imp['strasse'] != 'NULL')
							$ds=$imp['strasse'];
						if($imp['plz'] != 'NULL')
						{
							if(strlen($ds))
								$ds .= ', ';
							$ds .= $imp['plz'];
						}
						if($imp['ort'] != 'NULL')
						{
							if(strlen($ds))
								$ds .= ' ';
							$ds .= $imp['ort'];
						}
					}
					else if(isset($imp['kommunikation_txt']))
					{
						if($imp['kommunikation_txt'] != 'NULL')
						{
							$ds=substr($imp['kommunikation_txt'],0,72);
							if(strlen($ds) < strlen($imp['kommunikation_txt']))
								$ds .= '...';
						}
					}
					else
						$ds='[UNKNOWN IMP]';

					if(!strlen($ds))
						$ds='[EMPTY IMP]';
				}
				else
					$ds='[MISSING IMP]';
				if($l['elter'])
					$ds='&middot;&nbsp;'.trim($ds);
				else
					$ds='<b>'.trim($ds).'</b>';
				$inhalt .= '		<td'.$cls.' valign="top" width="100%" nowrap style="'.$xmltds.'">&nbsp;'.$ds.'&nbsp;</td>'.$nl;

				// Tabelle
				$inhalt .= '		<td'.$cls.' valign="top" nowrap style="'.$xmltds.'">&nbsp;'.$l['tabname'].'&nbsp;</td>'.$nl;

				// Status
				$fcz='';
				$fcc='green';
				if($ic == '3')	// Fehlerhafter Datensatz
					$fcc='red';
				else if($ic != '0')
				{
					$fcz=' italic';
					$fcc='grey';
				}
				$inhalt .= '		<td'.$cls.' valign="top" nowrap style="'.$xmltds.'"><font class="'.$fcc.$fcz.'">&nbsp;'.$istat[$l['import_status']].'&nbsp;</font></td>'.$nl;

				$inhalt .= '	</tr>'.$nl;
			}
			if($z)
			{
				$inhalt .= '	<tr>'.$nl;
				$inhalt .= '		<td colspan="5" style="'.$xmlels.'">&nbsp;</td>'.$nl;
				$inhalt .= '	</tr>'.$nl;
			}
			$inhalt .= '</table>'.$nl;

			// Meldungen
			$newpersright=false;
			if(!$iws && !$iwe)
				$mld=1751;	// Alle Datensätze wurden erfolgreich <B>temporär</B> importiert.
			else if($iws && !$iwe)
			{
				$mld=1752;	// Alle Datensätze wurden <B>erfolgreich</B> importiert.
				$newpersright=true;
			}
			else if(!$iws && $iwe)
				$mld=1753;	// Fehler beim importieren aller Datensätze!
			else
			{
				$mld=1754;	// Einige Datensätze konnten </B>nicht</B> importiert werden!
				$newpersright=true;
			}

			// Personenrechte neu ermitteln?
			if($newpersright)
				$persids=get_persons();

			error_msg($mld, false);
		}
		else
		{
			// 1750: Es wurden keine Datensätze importiert!
			$inhalt .= '<b class="red">'.meldung(1750, true).'</b>'.$nl;
			error_msg(1750, false);
		}
	break;
}

// Buttons ausblenden
$maxstep=sizeof($steps);
if($_step >= $maxstep-1)
	$Button_Next_nv=true;
if($_step != $maxstep-1)
	$Button_Import_nv=true;
if($_step != $maxstep)
	$Button_Start_nv=true;
if(($_step == 1) || (($_step == $maxstep) && !$err))
	$Button_Back_nv=true;

// Hidden
$hv=array('_step', '_xmlfile', '_xmltype', '_xmlsize', '_xmlpath');
if($rstep != 1)
	$hv[]='_group';
if($rstep != 3)
{
	$hv[]='_xmlelem';
	$hv[]='_dublette';
}
if($rstep != 4)
{
	$hv[]='_pers_n';
	$hv[]='_pers_c';
}
foreach($hv as $h)
	fieldHidden($h, ${$h});

// Überschrift
$maskenfeldwerte['ueberschrift']='~<b>'.$txt_schritt.' '.$_step.'/'.$maxstep.':</b> <b class="grey">'.$steps[$_step].'</b><hr class="fxhr" style="width:'.($mwidth-20).'px;"><br />';

// Inhalt anzeigen
$maskenfeldwerte['textanzeige']='~'.$inhalt;
?>