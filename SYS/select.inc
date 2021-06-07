<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : select.inc                                                   //
// Version     : 21.1                                                         //
// Begin       : 2020-08-20                                                   //
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
 * Function collection for select fields
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

$GLOBALS['__loaded_'.basename(__FILE__)]=true;


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * AUSWAHL-ARRAY
 *
 * @param various $arg_array - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function select_array($arg_array)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	extract($arg_array);
//fxDebug($arg_array, '$arg_array');

	// Prüfen, ob Array überschrieben werden soll
	$array_overwrite=$a_name.'_array';
	if(isset($GLOBALS[$array_overwrite]))
		return $GLOBALS[$array_overwrite];

	// SELECTFELD
	$selsql='';
	if($s_name || is_array($s_name))
	{
		// Punkt aus Selectwert extrahieren, falls vorhanden
		if(!$s_wert)
			$s_wert=$s_feld;
		$selwert_value_org=$s_wert;
		$pktpos=strpos($selwert_value_org, '.');
		if($pktpos)
			$selwert_value=substr($selwert_value_org, $pktpos+1);
		else
			$selwert_value=$selwert_value_org;

		// Wurde ein Array mitgegeben?
		if(is_array($s_name))
		{
			$selsql=$arg_array['a_name'];
			$selectarray=$s_name;
		}
		// Befinden wir uns im Nur-Anzeige-Modus?
		else if($a_change)
		{
			if($a_select)
			{
				$ai_select=(int)$a_select;
				if(($a_select == $ai_select) && (strlen($a_select) == strlen($ai_select)))
					$sv=$a_select;
				else
					$sv="'$a_select'";
				$selsql=$s_name;
				$wpos=db_subpos($selsql, ' where ', 1);
				// Wurde ein "where" gefunden...
				if($wpos)
					$selsql=substr($selsql, 0, $wpos)." WHERE ".$selwert_value_org.'='.$sv." AND".substr($selsql, $wpos+6);
				else
				{
					$opos=db_subpos($selsql, ' order by ', 1);
					// Wurde ein "order by" gefunden...
					if($opos)
						$selsql=substr($selsql, 0, $opos)." WHERE ".$selwert_value_org.'='.$sv;
					else
						$selsql .= " WHERE ".$selwert_value_org.'='.$sv;
				}
			}
		}
		// Normalen SELECT ausführen
		else
		{
			$selsql=$s_name;
			// Prüfen, ob WHERE-Bedingung mitgegeben wurde
			if(isset($GLOBALS[$arg_array['a_name'].'_where']))
			{
				$wpos=db_subpos($selsql, ' where ', 1);
				if($wpos)
				{
					$opos=db_subpos($selsql, ' order by ', 1);
					$ws_name=substr($selsql, 0, $wpos+7).$GLOBALS[$arg_array['a_name'].'_where'];
					if($opos)
						$ws_name .= substr($selsql, $opos);
					$selsql=$ws_name;
				}
			}
		}
	}
	// Nachschlagefeld
	else if($s_id)
	{
		$mandid=$GLOBALS['M_ID'];
		if(!$mandid)
			$mandid=$GLOBALS['fxpglobals']['client'];

		$mandbed="mandanten_id=0";
		// Bei Neuanlage Mandant-Grunddaten nur allgemeingültige Nachschlagewerte
		if(($GLOBALS['fxpglobals']['tr'] != 166) || ($GLOBALS['fxpglobals']['action'] != FXP_CREATE))
			$mandbed .= " OR mandanten_id=".$mandid;

		$f_id=$s_id;
		if($f_id == 2049)	// Abrechnungsart_Mat -> Abrechnungsart
			$f_id .= ',69';

		if($a_change)
		{
			if($a_select != '')
				$selsql="SELECT wertetabid, tabwert FROM wertetab WHERE (".$mandbed.") AND id_feld IN (".$f_id.") AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND wertetabid=".$a_select;
		}
		else
		{
			$exception="";
			if(($s_id == 667) || ($s_id == 696))	// Dokument-Kategorie/-Art
				$exception=" AND wertetabid<>2581";	// "fxp-intern" ausschliessen
			else if($s_id == 69)	// Abrechnungsart (Personen)
				$exception=" AND wertetabid NOT IN (".FXP_INV_MAT_HU.",".FXP_INV_DAYRATE.")";	// "Stunden- oder Mengenbasis" + "Tagessatzbasis" ausschliessen
			else if($s_id == 2049)	// Abrechnungsart (Material)
				$exception=" AND wertetabid NOT IN (".FXP_INV_HOUR.",".FXP_INV_DAYRATE.")";	// "Stundenbasis" + "Tagessatzbasis" ausschliessen

			$vex=$a_name.'_exception';
			if(isset($GLOBALS[$vex]))
				$exception .= " AND wertetabid NOT IN (".$GLOBALS[$vex].")";

			$selsql="SELECT wertetabid, tabwert FROM wertetab WHERE (".$mandbed.") AND id_feld IN (".$f_id.") AND id_sprache=".$GLOBALS['fxpglobals']['lang'].$exception." ORDER BY position, tabwert";
//fxDebug($selsql, '$selsql');
		}
	}

	// Liste und Array bereits vorhanden
	if($GLOBALS['fxptdata']['mskislist'] && strlen($selsql) && fxIsArray($GLOBALS['fxpselects']) && fxIsArray($GLOBALS['fxpselects']['sqlresults']) && fxIsArray($GLOBALS['fxpselects']['sqlresults'][$selsql]))
		$ret_array=$GLOBALS['fxpselects']['sqlresults'][$selsql];
	else
	{
		// Array aus Datenbank holen, falls noch nicht definiert
		if((!isset($selectarray) || !is_array($selectarray)) && strlen($selsql))
		{
			// selektierte Einträge IMMER anzeigen (derzeit testweise nur für Personenfeld in Vorgänge-Matrix)
			if(($GLOBALS['fxpglobals']['tr'] == 189) && ($arg_array['a_name'] == 'Personen_ID'))
			{
				if(is_array($arg_array['a_select']))
					$selectedValues=implode(', ', $arg_array['a_select']);
				else if(strlen($arg_array['a_select']))
					$selectedValues=$arg_array['a_select'];
				else
					$selectedValues=-666;
				$opos=db_subpos($selsql, ' order by ', 1);
				$selsql=substr($selsql, 0, $opos).' OR '.$arg_array['s_wert'].' IN ('.$selectedValues.')'.substr($selsql, $opos);
			}
			$selectarray=db_values($selsql, '*');
//echo '<br />Führe für Feld <b>"'.$arg_array['a_name'].'"</b> folgenden SQL-Call aus:<br /> => '.$selsql;
		}
		// Array nicht vorhanden -> Abbruch
		if(!isset($selectarray) || !is_array($selectarray))
			return '';

		// Function parsen und $GLOBALS ersetzen
		if($s_func)
			$s_func=str_replace('$GLOBALS[$selfeld]', '"'.$s_feld.'"', $s_func);

		// Rückgabe-Array aufbauen
		if(isset($GLOBALS[$arg_array['a_name'].'_return']))
			$ret_array=$selectarray;
		else
		{
			$ret_array=array();
			foreach($selectarray as $selectwerte)
			{
				if($s_func)
				{
					eval($s_func);
				}

				if(fxIsArray($selectwerte))
				{
					if($s_name || is_array($s_name))
						$ret_array[trim($selectwerte[$selwert_value])]=trim($selectwerte[$s_feld]);
					else
						$ret_array[trim($selectwerte['wertetabid'])]=trim($selectwerte['tabwert']);
				}
			}
		}
		// Liste, d.h. Array merken
		if($GLOBALS['fxptdata']['mskislist'] && fxIsArray($ret_array))
		{
			if(!fxIsArray($GLOBALS['fxpselects']))
				$GLOBALS['fxpselects']=array();
			if(!fxIsArray($GLOBALS['fxpselects']['sqlresults']))
				$GLOBALS['fxpselects']['sqlresults']=array();
			$GLOBALS['fxpselects']['sqlresults'][$selsql]=$ret_array;
		}
	}

	// Wurde ein Werte in FELDNAME_valuedisabled mitgegeben
	if(fxIsArray($ret_array) && isset($GLOBALS[$arg_array['a_name'].'_valuedisabled']))
		unset($ret_array[$GLOBALS[$arg_array['a_name'].'_valuedisabled']]);

	$ptc=0;
	$pf_name=$a_name.$a_index;
	if(!isset($GLOBALS['noselpopptype']) && isset($GLOBALS['selpopptype']) && isset($GLOBALS['selpopptype'][$pf_name]) && fxIsArray($ret_array))
	{
//fxDebug($ret_array,'$ret_array');
		// Zuletzt gesetzte Checkboxen wieder herstellen
		$psps=array();
		if(fxIsArray($GLOBALS['fxptarray']) && isset($GLOBALS['fxptarray']['psps_'.$pf_name]) && strlen($GLOBALS['fxptarray']['psps_'.$pf_name]))
			$psps=explode('|',$GLOBALS['fxptarray']['psps_'.$pf_name]);
//fxDebug($psps,'psps_'.$pf_name);

		$arg=array();
		if(fxIsArray($GLOBALS[$pf_name.'_arg']))
			$arg=$GLOBALS[$pf_name.'_arg'];
//fxDebug($arg,'$arg');

		$rar=array();
		$mpc='-';
		$spc='-';
		$tsc='-';
		$mpcnt=0;
		$pds=$a_name.'_ds';
		if(strlen($a_defsel) || (isset($GLOBALS[$pds]) && $GLOBALS[$pds]))
			$mpcnt++;
		foreach($ret_array as $rid => $entrytext)
		{
			$est=substr($entrytext,-3);
			if($est == '**}')
			{
				$mpc='1';
//echo(__LINE__.': $mpcnt='.$mpcnt.': $rid='.$pid.', $entrytext='.$entrytext.'<br />');
				if($mpcnt && !isset($GLOBALS['app_lang']) && (!isset($arg['mp_only']) || !$arg['mp_only']))
					$rar[-$rid]='<d>';
				$mpcnt++;
			}
			else if($est == '{*}')
				$spc='1';
			else
				$tsc='1';
			$rar[$rid]=$entrytext;
		}
		if($mpc != '-')
		{
			if(sizeof($psps) && isset($psps[0]))
				$mpc=$psps[0];
			$ptc++;
		}
		if($spc != '-')
		{
			if(sizeof($psps) && isset($psps[1]))
				$spc=$psps[1];
			$ptc++;
		}
		if($tsc != '-')
		{
			if(sizeof($psps) && isset($psps[2]))
				$tsc=$psps[2];
			$ptc++;
		}
//echo('<b>'.$pf_name.'</b> -- $mpc='.$mpc.', $spc='.$spc.', $tsc='.$tsc.' -&gt; $ptc='.$ptc.'<hr />');
		if($ptc > 1)
			echo('<input id="selpopptype_'.$pf_name.'" name="selpopptype_'.$pf_name.'" type="hidden" value="'.$mpc.'|'.$spc.'|'.$tsc.'">');

		$ret_array=$rar;
	}
	return $ret_array;
}

/**
 * AUSWAHL-FUNKTION
 *
 * @param various $a_return  - Mandatory parameter: ???PARAMETER???
 * @param various $a_array   - Mandatory parameter: ???PARAMETER???
 * @param various $arg_array - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function convert_array($a_return, $a_array, $arg_array)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Variablen
	extract($arg_array);

	$msc=$GLOBALS['_maskcounter'];
	$msf=(int)$GLOBALS['msfcnt'];

	$bzkey='';
	$bzlnr='';
	if(strlen($a_index))
	{
		$bzexa=explode('[', substr($a_index, 1));
		if(sizeof($bzexa) == 1)
			$bzlnr=substr($bzexa[0],0,-1);
		else if(sizeof($bzexa) > 1)
		{
			$bzkey=substr($bzexa[0],0,-1);
			$bzlnr=substr($bzexa[1],0,-1);
		}
	}
	$s_name=$a_name.$a_index;
	$i_name=$a_name.$bzlnr;
	$s_disp=array();
	$s_ajax=false;
	$s_multi=false;
	if(((int)$a_width < 0) || ((int)$a_height > 0))
		$s_multi=true;
//echo('$a_width='.$a_width.', $a_height='.$a_height.' -- $s_multi='.$s_multi.'<br />');

	// Prüfen, ob Feld disabled werden soll
	$dis=false;
	$disable_it=$a_name.'_dis';
	if(isset($GLOBALS[$disable_it]) && $GLOBALS[$disable_it])
		$dis=true;
	if(!$dis && strlen($bzkey))
	{
		$disable_it=$i_name.'_dis';
		if(isset($GLOBALS[$disable_it]) && $GLOBALS[$disable_it])
			$dis=true;
		else
		{
			$disable_it='row'.$bzlnr.'_dis';
			if(isset($GLOBALS[$disable_it]) && $GLOBALS[$disable_it])
				$dis=true;
		}
	}

	// Prüfen, ob JavaScript Funktion hinzugefügt werden soll
	$jsfunc_it=$a_name.'_jf';
	if(!isset($jsfunc) && isset($GLOBALS[$jsfunc_it]))
		$jsfunc=$GLOBALS[$jsfunc_it];
	if(!isset($jsfunc) && strlen($bzkey))
	{
		$jsfunc_it=$i_name.'_jf';
		if(isset($GLOBALS[$jsfunc_it]))
			$jsfunc=$GLOBALS[$jsfunc_it];
	}

	// Prüfen, ob Person im Auswahlfeld stehen sollte, z.B. Ressource in 23: Vorgänge (Detail) oder 189: Vorgänge (Matrix)
	if(($GLOBALS['fxpglobals']['tr'] == 23) || ($GLOBALS['fxpglobals']['tr'] == 189))
	{
		$ias=0;
		if(fxIsArray($a_array) && isset($a_name) && ($a_name == 'Personen_ID') && isset($a_select))
			$ias=(int)$a_select;
		if($ias && !isset($a_array[$ias]))
		{
			$sql="SELECT personen_id, pname, vorname, personen_initialen, geschlecht, personentyp, inaktiv FROM personen WHERE personen_id=".$ias." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$p=db_values($sql);
//fxDebug($p, $sql);
			if(fxIsArray($p))
			{
				$with_icons=false;
				$mps=getSettingValue('max_pic_sel');
				if(is_array($a_array) && (!$mps || (sizeof($a_array) < $mps)))
					$with_icons=true;
				$a_array[$ias]=persArray2Text($p,$with_icons);
				asort($a_array);
			}
		}
	}

	// Auswahlfeld: SELECT-Header erstellen
	$errcode=errorCode($a_name,$GLOBALS['_masknumber'],$a_index);
	$style=$errcode['style'];
	if($a_dis === 'hide')
	{
		$a_dis=false;
		$style .= 'display:none;';
	}
	// Spaltenauswahl-Popup
	if(substr($s_name,0,7) == 'rcmain_')
		$style .= 'top:-1px;';
	// Tooltip
	if($errcode['id'])
		$tooltip=maskErrorTooltip($errcode['id'], $errcode['mnr'], $errcode['nr'], $errcode['txt']);
	else
	{
		$tooltip='';
		$tt=$a_name.'_tt';
		if(isset($GLOBALS[$tt]) && strlen($GLOBALS[$tt]))
			$tooltip=$GLOBALS[$tt];
		if(!strlen($tooltip) && strlen($bzkey))
		{
			$tt=$i_name.'_tt';
			if(isset($GLOBALS[$tt]) && strlen($GLOBALS[$tt]))
				$tooltip=$GLOBALS[$tt];
		}
		if(strlen($tooltip))
		{
			$tooltip=' tooltip="'.$tooltip.'"';
			$style .= 'cursor:help;';
		}
	}
	if($a_width || $GLOBALS['mtd_width'])
	{
		if($a_width)
			$width=(int)(abs($a_width)*$GLOBALS['msk_wfac']);
		else
			$width=$GLOBALS['mtd_width'];
		$style .= 'width:'.$width.'px;';
	}
	// Filtermarkierung?
	$set_filter=$a_name.'_flt';
//echo(__FILE__.': <b>'.$set_filter.'</b> = '.$GLOBALS[$set_filter].'<br />');
	$flt_col='';
	if($GLOBALS[$set_filter] & 512)	// Filter gesetzt für dieses Feld oder explizit angegeben
	{
		if(strtolower(substr($GLOBALS['fxpglobals']['settings']['filterrahmen'], 0, 1)) == 'j')
		{
			if($GLOBALS[$set_filter] & 2048)		// Filterfeld aktiv und mit Einschränkungen,	dann "grüner" Rahmen
				$flt_col='009f6b';
			else if($GLOBALS[$set_filter] & 1024)	// Filterfeld inaktiv,							dann "roter" Rahmen
				$flt_col='e1001a';
			else									// Filterfeld aktiv aber ohne Einschränkungen,	dann "blauer" Rahmen
				$flt_col='006b9f';
		}
		else
			$flt_col='009f6b';						// immer grün lt. Hr. Binder am 22.06.2006
	}

	$tmp=$GLOBALS['trtabs'].'<div id="'.$s_name.'" name="'.$s_name.'" fxform="'.$GLOBALS['_form'].'" class="fxfs';
	if($s_multi)
		$tmp .= 'm';
	else
		$tmp .= 'l';
	if($a_muss)
		$tmp .= 'm';
	$classadd=$a_name.'_classadd';
	if(isset($GLOBALS[$classadd]) && strlen(trim($GLOBALS[$classadd])))
		$tmp .= ' '.trim($GLOBALS[$classadd]);
	$tabindex=0;
	if($dis)
	{
		$tmp .= 'g';
		$tabindex=-1;
	}
	$tmp .= '" tabindex="'.$tabindex.'" svalue="{dkey}"';
	if($s_multi)
	{
		if(($a_height != '') && ($a_height != '1'))
			$afs=$a_height;
		else
			$afs=1;
		$style .= 'height:'.($afs*16-2).'px;';
//echo('<b>Multiselect</b> '.$s_name.': $afs='.$afs.'<br />');
	}
	$tmp .= $tooltip;
	if($dis)
	{
		$udf=$i_name.'_ud';
		if(isset($GLOBALS[$udf]) && $GLOBALS[$udf])
			$tmp .= ' ud="1"';
	}
	$style .= 'text-align:left;';
	if(strlen($flt_col))
		$style .= 'border-right:1px solid #'.$flt_col.';border-bottom:1px solid #'.$flt_col.';';
	// Add style?
	$stadd=$a_name.'_'.$msc.'_styleadd';
	if(!isset($GLOBALS[$stadd]))
		$stadd=$a_name.'_styleadd';
	if(isset($GLOBALS[$stadd]))
		$style .= $GLOBALS[$stadd];
	$tmp .= ' style="'.$style.'{dstyle}"';
	if(!$dis && isset($jsfunc) && ($jsfunc != false))
		$tmp .= ' '.$jsfunc;
	else if(strlen($GLOBALS['fxpglobals']['browsermobile']))
		$tmp .= ' '.fxf_jsFunction('#mouseDown');
 	$tmp .= '>';
	if(!$s_multi)
		$tmp .= '{dval}<div class="fxfslc"'.$tooltip.' style="cursor:pointer;"><span class="fxfsli"></span></div>';
	else
		$tmp .= '{mval}';
	$tmp .= '</div>'.$GLOBALS['nl'];
//echo('<pre>'.fxHtmlEncode($tmp).'</pre>');

	// Auswahlfeld: SELECT-Default erstellen
	$fov='';
	$GLOBALS['fxpselects'][$s_name]=array();

	$ds_array=$i_name.'_ds_array';
//echo('$s_name='.$s_name.', $i_name='.$i_name.', $ds_array='.$ds_array.'<br />');
	if(isset($GLOBALS[$ds_array]) && is_array($GLOBALS[$ds_array]))
	{
		foreach($GLOBALS[$ds_array] as $ov => $val)
		{
			$tmp_v='';
			if(strlen($val) && (gettype($val) == 'string'))
				$tmp_v=$val;
			else if($val)
				$tmp_v=meldung($val, true);

			if(strlen($tmp_v))
			{
				$GLOBALS['fxpselects'][$s_name][$ov]=$tmp_v;
				if(!strlen($fov))
					$fov=$ov;
				$s_disp[$ov]=$tmp_v;
			}
		}
	}

	if(!sizeof($GLOBALS['fxpselects'][$s_name]))
	{
		if(strlen($a_defsel))
		{
			if(strlen($a_defsel) && (gettype($a_defsel) == 'string'))
				$def=$a_defsel;
			else if($a_defsel)
				$def=meldung($a_defsel, true);
		}
		else if($a_muss) // Muss-Auswahlfeld
			$def=meldung(684, true);	// -auswählen-
		else
			$def=meldung(794, true);	// -leer-

		if(strlen($def))
		{
			// bei Auswahlfeldern der Matrix nur selektierte Option anzeigen
			$GLOBALS['fxpselects'][$s_name][0]=$def;
			if(!strlen($fov))
				$fov=0;
			$s_disp[0]=$def;
		}
	}

	// Auswahlfeld: SELECT-Body erstellen
	$addon=$a_name.'_addon';
	if(isset($GLOBALS[$addon]) && is_array($GLOBALS[$addon]))
	{
		foreach($GLOBALS[$addon] as $key => $value)
		{
			$GLOBALS['fxpselects'][$s_name][$key]=$value;
			if(!strlen($fov))
				$fov=$key;
			if(!$s_ajax || ($key == $a_select))
				$s_disp[$key]=$value;
		}
	}

	if(fxIsArray($a_array))
	{
		foreach($a_array as $key => $value)
		{
			$GLOBALS['fxpselects'][$s_name][$key]=$value;
			if(!strlen($fov))
				$fov=$key;
			if(!$s_ajax || ($key == $a_select))
				$s_disp[$key]=$value;
		}
	}

	// Kein Eintrag, dann -leer-
	if(!sizeof($GLOBALS['fxpselects'][$s_name]))
	{
		$value=meldung(794, true);

		$GLOBALS['fxpselects'][$s_name][0]=$value;
		$s_disp[0]=$value;
	}
	if(!sizeof($s_disp))
	{
		if(strlen($a_select) && isset($GLOBALS['fxpselects'][$s_name][$a_select]))
			$s_disp[$a_select]=$GLOBALS['fxpselects'][$s_name][$a_select];
		else if(strlen($fov) && isset($GLOBALS['fxpselects'][$s_name][$fov]))
			$s_disp[$fov]=$GLOBALS['fxpselects'][$s_name][$fov];
		else if(isset($GLOBALS['fxpselects'][$s_name][0]))
			$s_disp[0]=$GLOBALS['fxpselects'][$s_name][0];
	}

	// Gesamtstring des Auswahlfeldes aufbauen
	$s_array=array();
	if(!$a_change)
	{
		if($dis && ($dis !== 'yes'))
			$tmp .= fieldHidden($s_name,$a_select);
		if(!$dis)
			$tmp .= fieldHidden('so_'.$s_name,0);

		if(sizeof($s_disp))
		{
			// Temporäres Select-Array erstellen
			$tca=array();
			$tsa=array();
			if(!is_array($a_select) && strlen($a_select))
			{
				$asq=$a_select.'"';
				$ass=(string)$a_select;
				$las=strlen($ass)+1;
				foreach($s_disp as $skey => $svalue)
				{
					$tca[substr($skey, 0, $las)]=$skey;
					$tsa[(string)$skey]=$skey;
				}
			}

			$first_option=true;
			foreach($s_disp as $key => $value)
			{
				$selected='';
				if(is_array($a_select))
				{
					foreach($a_select as $akey)
					{
						$lak=strlen($akey)+1;
						if((substr($key, 0, $lak) === (string)$akey) || ((string)$key == (string)$akey))
						{
							$selected=' SELECTED';
//echo(__LINE__.' ['.$s_name.']: $key:'.$key.' = $akey:'.$akey.'<br />');
							break;
						}
					}
				}
				else
				{
					$ak_found='';
					if(strlen($a_select))
					{
						if(isset($tca[$asq]))
							$ak_found=$tca[$asq];
						else if(isset($tsa[$ass]))
							$ak_found=$tsa[$ass];
					}

					if(strlen($a_select) && strlen($ak_found))
					{
						if(((string)$key == (string)$ak_found) && (strlen($key) == strlen($ak_found)))
						{
							$selected=' SELECTED';
//echo(__LINE__.' ['.$s_name.']: $key:'.$key.' = $ak_found:'.$ak_found.'<br />');
						}
					}
					else if(strlen($fov) && isset($s_disp[$fov]))
					{
						if($key == $fov)
						{
							$selected=' SELECTED';
//echo(__LINE__.' ['.$s_name.']: $key:'.$key.' = $fov:'.$fov.'<br />');
						}
					}
					else if($first_option)
					{
						$selected=' SELECTED';
//echo(__LINE__.' ['.$s_name.']: firstoption='.$key.'<br />');
					}
				}
				if(strlen($selected))
				{
//echo('<b>'.$s_name.'</b>: $key='.$key.', $value='.fxHtmlEncode($value).'<hr>');
					$first_option=false;
					$s_array[$key]=$value;
				}
			}
		}

		$GLOBALS['msfcnt']=$msf + 1;
	}
	else if(strlen($a_select) && isset($s_disp[$a_select]))
	{
//echo('<b>hidden: '.$s_name.'</b>: $key='.$a_select.'<hr>');
		$st=selGetStyle($s_disp[$a_select]);
		$tmp='<div style="'.$st['st'].'">'.$st['tx'].'</div>'.fieldHidden($s_name,$a_select);
	}
	else if(isset($s_disp[0]))
	{
//echo('<b>hidden: '.$s_name.'</b>: $key=0<hr>');
		$tmp=$s_disp[0].fieldHidden($s_name,0);
	}

	// Multiselect
	$mval='';
	if($s_multi)
		$mval=multiSelEntries($s_name, $s_disp, $s_array);
	else
		$mval='';

	// Save selected
	$dkey='';
	$dval='';
	$dstyle='';
	if(fxIsArray($s_array))
	{
		foreach($s_array as $key => $value)
		{
			$s_array[$key]=$value;
			if(strlen($dkey))
			{
				$dkey .= '|';
				$dval .= '|';
			}
			$dkey .= $key;

			$st=selGetStyle($value);
//fxDebug($st,$value);
			$dval .= $st['tx'];

			if(!$s_multi)
			{
				$dstyle .= $st['st'];
				break;
			}
		}
//fxDebug($s_array, '$s_array: $s_name='.$s_name.', $dkey='.$dkey);

		$dskey=$dkey;
		if($s_multi)
			$dskey=explode('|',$dskey);
		fxSession('t','p',$s_name,$dskey);
	}

	// Quick javascript reference
	$jsqex=array('Fehltagsart'=>true, 'kdatei'=>true);
	$sn5=substr($s_name,0,5);
	if(!isset($jsqex[$s_name]) && ($sn5 != 'rcmai') && ($sn5 != 'rcsel'))
	{
//		echo('<b class="blue">sel_'.$s_name.'</b><br />');
		selEntriesJS($s_name, $GLOBALS['fxpselects'][$s_name]);
	}

//echo('<b><u>'.$s_name.'</u></b><br /><b class="blue">dkey</b><pre>'.fxHtmlEncode($dkey).'</pre><b class="blue">dstyle</b><pre>'.fxHtmlEncode($dstyle).'</pre><b class="blue">dval</b><pre>'.fxHtmlEncode($dval).'</pre><b class="blue">mval</b><pre>'.fxHtmlEncode($mval).'</pre>');
	$tmp=strtr($tmp, array('{dkey}'=>$dkey, '{dstyle}'=>$dstyle, '{dval}'=>$dval, '{mval}'=>$mval));
//echo('<b><u>'.$s_name.'</u></b><pre>'.fxHtmlEncode($tmp).'</pre>');

	// HTML-Auswahlfeld zurückgeben
	return $tmp;
}

/**
 * ???FUNCTION???
 *
 * @param various $t - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function selGetStyle($t)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$r=array('st'=>"", 'bg'=>"", 'tx'=>$t, 'tt'=>"", 'dis'=>false);

	$r['tx']=str_replace('&apos;', "'", $r['tx']);

	// Disabled
	$i=stripos($r['tx'], '&lt;d&gt;');
	if($i !== false)
	{
		$r['dis'] = true;
		$r['tx'] = preg_replace(array('/\&lt\;d\&gt\;/i', '/\&lt\;\/d\&gt\;/i'), array('', ''), $r['tx']);
	}
	else
	{
		$i=stripos($r['tx'], '<d>');
		if($i !== false)
		{
			$r['dis'] = true;
			$r['tx'] = preg_replace(array('/\<d\>/i', '/\<\/d\>/i'), array('', ''), $r['tx']);
		}
	}

	// Bold
	$i=stripos($r['tx'], '&lt;b&gt;');
	if($i !== false)
	{
		$r['st'] .= 'font-weight:bold;';
		$r['tx'] = preg_replace(array('/\&lt\;b\&gt\;/i', '/\&lt\;\/b\&gt\;/i'), array('', ''), $r['tx']);
	}
	else
	{
		$i=stripos($r['tx'], '<b>');
		if($i !== false)
		{
			$r['st'] .= 'font-weight:bold;';
			$r['tx'] = preg_replace(array('/\<b\>/i', '/\<\/b\>/i'), array('', ''), $r['tx']);
		}
	}

	// Underline
	$i=stripos($r['tx'], '&lt;u&gt;');
	if($i !== false)
	{
		$r['st'] .= 'text-decoration:underline;';
		$r['tx'] = preg_replace(array('/\&lt\;u\&gt\;/i', '/\&lt\;\/u\&gt\;/i'), array('', ''), $r['tx']);
	}
	else
	{
		$i=stripos($r['tx'], '<u>');
		if($i !== false)
		{
			$r['st'] .= 'text-decoration:underline;';
			$r['tx'] = preg_replace(array('/\<u\>/i', '/\<\/u\>/i'), array('', ''), $r['tx']);
		}
	}

	// Italic
	$i=stripos($r['tx'], '&lt;i&gt;');
	if($i !== false)
	{
		$r['st'] .= 'font-style:italic;color:#444444;';
		$r['tx'] = preg_replace(array('/\&lt\;i\&gt\;/i', '/\&lt\;\/i\&gt\;/i'), array('', ''), $r['tx']);
	}
	else
	{
		$i=stripos($r['tx'], '<i>');
		if($i !== false)
		{
			$r['st'] .= 'font-style:italic;color:#444444;';
			$r['tx'] = preg_replace(array('/\<i\>/i', '/\<\/i\>/i'), array('', ''), $r['tx']);
		}
	}

	// Background
	$i=stripos($r['tx'], '&lt;bg#');
	if($i !== false)
	{
		$r['bg'] = '#'.substr($r['tx'],$i+7,6);
		$r['st'] .= 'background:'.$r['bg'].';';
		$r['tx'] = preg_replace('/\&lt\;bg\#......\&gt\;/i', '', $r['tx']);
	}
	else
	{
		$i=stripos($r['tx'], '<bg#');
		if($i !== false)
		{
			$r['bg'] = '#'.substr($r['tx'],$i+4,6);
			$r['st'] .= 'background:'.$r['bg'].';';
			$r['tx'] = preg_replace('/\<bg\#......\>/i', '', $r['tx']);
		}
	}

	// Border Bottom
	$i=stripos($r['tx'], '&lt;bb&gt;');
	if($i !== false)
	{
		$r['st'] .= 'border-bottom:1px solid #888888;';
		$r['tx'] = preg_replace('/\&lt\;bb\&gt\;/i', '', $r['tx']);
	}
	else
	{
		$i=stripos($r['tx'], '<bb>');
		if($i !== false)
		{
			$r['st'] .= 'border-bottom:1px solid #888888;';
			$r['tx'] = preg_replace('/\<bb\>/i', '', $r['tx']);
		}
	}

	// Image
	$i=stripos($r['tx'], '&lt;img');
	if($i !== false)
	{
		$r['tx'] = preg_replace(array('/\&lt\;img/i', '/\/\&gt\;/i'), array('<img', '/>'), $r['tx']);
	}
	else
	{
		$i=stripos($r['tx'], '<img');
		if($i !== false)
		{
			$r['tx'] = preg_replace(array('/\<img/i', '/\/\>/i'), array('<img', '/>'), $r['tx']);
		}
	}

	// Tooltip
	$i=stripos($r['tx'], '&lt;tt&gt;');
	if($i !== false)
	{
		$ie=stripos($r['tx'], '&lt;/tt&gt;', $i+10);
		$r['tt'] = substr($r['tx'], $i+10, $ie-$i-10);
		$r['tx'] = preg_replace('/\&lt\;tt\&gt\;.*\&lt\;\/tt\&gt\;/i', '', $r['tx']);
	}
	else
	{
		$i=stripos($r['tx'], '<tt>');
		if($i !== false)
		{
			$ie=stripos($r['tx'], '</tt>', $i+4);
			$r['tt'] = substr($r['tx'], $i+4, $ie-$i-4);
			$r['tx'] = preg_replace('/\<tt\>.*\<\/tt\>/i', '', $r['tx']);
		}
	}

	// Blue
	$i=stripos($r['tx'], '&lt;blue&gt;');
	if($i !== false)
	{
		$r['st'] .= 'color:#006b9f;';
		$r['tx'] = preg_replace(array('/\&lt\;blue\&gt\;/i', '/\&lt\;\/blue\&gt\;/i'), array('', ''), $r['tx']);
	}
	else
	{
		$i=stripos($r['tx'], '<blue>');
		if($i !== false)
		{
			$r['st'] .= 'color:#006b9f;';
			$r['tx'] = preg_replace(array('/\<blue\>/i', '/\<\/blue\>/i'), array('', ''), $r['tx']);
		}
		else
		{
			// Red
			$i=stripos($r['tx'], '&lt;red&gt;');
			if($i !== false)
			{
				$r['st'] .= 'color:#e1001a;';
				$r['tx'] = preg_replace(array('/\&lt\;red\&gt\;/i', '/\&lt\;\/red\&gt\;/i'), array('', ''), $r['tx']);
			}
			else
			{
				$i=stripos($r['tx'], '<red>');
				if($i !== false)
				{
					$r['st'] .= 'color:#e1001a;';
					$r['tx'] = preg_replace(array('/\<red\>/i', '/\<\/red\>/i'), array('', ''), $r['tx']);
				}
				else
				{
					// Green
					$i=stripos($r['tx'], '&lt;green&gt;');
					if($i !== false)
					{
						$r['st'] .= 'color:#009f6b;';
						$r['tx'] = preg_replace(array('/\&lt\;green\&gt\;/i', '/\&lt\;\/green\&gt\;/i'), array('', ''), $r['tx']);
					}
					else
					{
						$i=stripos($r['tx'], '<green>');
						if($i !== false)
						{
							$r['st'] .= 'color:#009f6b;';
							$r['tx'] = preg_replace(array('/\<green\>/i', '/\<\/green\>/i'), array('', ''), $r['tx']);
						}
						else
						{
							// Grey
							$i=stripos($r['tx'], '&lt;grey&gt;');
							if($i !== false)
							{
								$r['st'] .= 'color:#777777;';
								$r['tx'] = preg_replace(array('/\&lt\;grey\&gt\;/i', '/\&lt\;\/grey\&gt\;/i'), array('', ''), $r['tx']);
							}
							else
							{
								$i=stripos($r['tx'], '<grey>');
								if($i !== false)
								{
									$r['st'] .= 'color:#777777;';
									$r['tx'] = preg_replace(array('/\<grey\>/i', '/\<\/grey\>/i'), array('', ''), $r['tx']);
								}
							}
						}
					}
				}
			}
		}
	}

	if($GLOBALS['fxpglobals']['lang'] == 1)
		$i=stripos($r['tx'], '{inaktiv}');
	else
		$i=stripos($r['tx'], '{inactive}');
	if($i !== false)
		$r['st'] .= 'font-style:italic;color:#888888;';

	if($GLOBALS['fxpglobals']['lang'] == 1)
		$i=stripos($r['tx'], '{gesperrt}');
	else
		$i=stripos($r['tx'], '{locked}');
	if($i !== false)
		$r['st'] .= 'font-style:italic;color:#e1001a;';

	$l3=substr($r['tx'],-3);
	if($l3 == '**}')
		$r['st'] .= 'font-style:italic;font-weight:bold;';
	else if($l3 == '{*}')
		$r['st'] .= 'font-style:italic;color:#333333;';

//fxDebug($r,'$r');
	return $r;
}

/**
 * ???FUNCTION???
 *
 * @param various $id   - Mandatory parameter:                     ???PARAMETER???
 * @param various $sarr - Mandatory parameter:                     ???PARAMETER???
 * @param array   $varr - Optional parameter (default = array()):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function multiSelEntries($id, $sarr, $varr=array())
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$lis='';
	$firstsel='';
	$sec=0;
	if(fxIsArray($sarr))
	{
		$md='';
		if(strlen($GLOBALS['fxpglobals']['browsermobile']))
			$md=' '.fxf_jsFunction('#mouseDown');
		$lc=1;
		foreach($sarr as $key => $value)
		{
			$sel='';
			if(fxIsArray($varr))
			{
				foreach($varr as $ck => $cv)
				{
					if($ck === $key)
					{
						$sel='sel';
						if(!strlen($firstsel))
							$firstsel=$lc-1;
						break;
					}
				}
			}
			else if($lc == 1)
			{
				$sel='sel';
				$firstsel=0;
			}

			$st=selGetStyle($value);
			$tt='';
			if(strlen($st['tt']))
			{
				$tt=' tooltip="'.$st['tt'].'"';
				$st['st'] .= 'cursor:help;';
			}
			$s='';
			if(strlen($st['st']))
				$s=' style="'.$st['st'].'"';

			if($st['dis'])	// Disabled?
				$lis .= '	<li id="fspmo_'.$id.'-'.$lc.'" class="sdisabled"'.$tt.'>'.$st['tx'].'</li>'.$GLOBALS['nl'];
			else
				$lis .= '	<li id="fspmo_'.$id.'-'.$lc.'" class="msentry'.$sel.'" svalue="'.$key.'"'.$tt.$s.$md.'>'.$st['tx'].'</li>'.$GLOBALS['nl'];
			$lc++;
			$sec++;
		}
	}
	$mval  = '<ul id="msul_'.$id.'" firstsel="'.$firstsel.'">'.$GLOBALS['nl'];
	$mval .= $lis;
	$mval .= '</ul><div id="msec_'.$id.'" style="display:none;">'.$sec.'</div>'.$GLOBALS['nl'];
//echo('<b><u>msul_'.$fname.'</u></b><pre style="width:1024px;white-space:normal;">'.fxHtmlEncode($mval).'</pre>');

	return $mval;
}

/**
 * Create select entries as javascript parameter
 *
 * @param various $fname     - Mandatory parameter:                   ???PARAMETER???
 * @param various $source    - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $mandatory - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $withempty - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $ret       - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function selEntriesJS($fname, $source, $mandatory=false, $withempty=false, $ret=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$csv='';
	$sec=0;

	if(is_integer($source))	// Wertetab
	{
		if($withempty)
		{
			if($mandatory)
				$csv='0'.$GLOBALS['_divstr'][0].'&lt;i&gt;'.meldung(684);
			else
				$csv='0'.$GLOBALS['_divstr'][0].'&lt;i&gt;'.meldung(794);
		}

		$idf=$source;
		if($idf == 2049)	// Abrechnungsart_Mat -> Abrechnungsart
			$idf .= ',69';

		$sql="SELECT wertetabid, tabwert FROM wertetab WHERE id_feld IN (".$idf.") AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id IN(0,".$GLOBALS['fxpglobals']['client'].")";
		if($source == 69)			// Keine "Stunden- oder Mengenbasis" + "Tagessatzbasis" bei Abrechnungsart (Personen)
			$sql .= " AND wertetabid NOT IN (".FXP_INV_MAT_HU.",".FXP_INV_DAYRATE.")";
		else if($source == 2049)	// Keine "Stundenbasis" + "Tagessatzbasis" bei Abrechnungsart (Material)
			$sql .= " AND wertetabid NOT IN (".FXP_INV_HOUR.",".FXP_INV_DAYRATE.")";
		$sql .= " ORDER BY position,tabwert";
		$arr=db_values($sql,'*');
		if(fxIsArray($arr))
		{
			if($withempty)
				$csv .= $GLOBALS['_divstr'][1].'-1'.$GLOBALS['_divstr'][0].'&lt;d&gt;';
			foreach($arr as $a)
			{
				if(strlen($csv))
					$csv .= $GLOBALS['_divstr'][1];
				$csv .= $a['wertetabid'].$GLOBALS['_divstr'][0].$a['tabwert'];
				$sec++;
			}
		}
	}
	else
	{
		$es=false;
		if($withempty && (!fxIsArray($source) || !isset($source[0])))
		{
			$es=true;
			if($mandatory)
				$csv='0'.$GLOBALS['_divstr'][0].'&lt;i&gt;'.meldung(684);
			else
				$csv='0'.$GLOBALS['_divstr'][0].'&lt;i&gt;'.meldung(794);
		}

		if(fxIsArray($source))
		{
			$add=array();
			$sa=false;
			if(($fname == 'hr') || (($fname == 'Personen_ID') && ($GLOBALS['fxpglobals']['tr'] == 23)))
			{
				$sa=true;
				if($es)
					$csv .= $GLOBALS['_divstr'][0].'0'.$GLOBALS['_divstr'][0].'0'.$GLOBALS['_divstr'][0];

				$paa=array();

				// Standard EK/VK
				$sql="SELECT personen_id, ek, vk FROM artikel WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND (sachmittel_id IS NULL or sachmittel_id=0) AND (archiv IS NULL OR archiv=0)";
				$ava=db_values($sql,'*');
				if(fxIsArray($ava))
				{
					foreach($ava as $a)
					{
						$pers=$a['personen_id'];
						if(!isset($paa[$pers]))
							$paa[$pers]=array();
						$paa[$pers]['ek']=$a['ek'];
						$paa[$pers]['vk']=$a['vk'];
					}
				}

				// Fähigkeiten
				$sql="SELECT personen_id, maart_id, mafaehigkeit_id FROM ma_art_faehigkeiten WHERE mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$ava=db_values($sql,'*');
				if(fxIsArray($ava))
				{
					$rsa=array();
					foreach($ava as $a)
					{
						$pers=$a['personen_id'];
						if(!isset($paa[$pers]))
							$paa[$pers]=array();
						if(!isset($paa[$pers]['ma']))
							$paa[$pers]['ma']=array();
						$paa[$pers]['ma'][$a['maart_id']]=true;
						if(!isset($paa[$pers]['mf']))
							$paa[$pers]['mf']=array();
						$paa[$pers]['mf'][$a['mafaehigkeit_id']]=true;
					}
				}

				if(sizeof($paa))
				{
					foreach($paa as $pers => $a)
					{
						$ek=0;
						if(isset($a['ek']))
							$ek=$a['ek'];
						$vk=0;
						if(isset($a['vk']))
							$vk=$a['vk'];
						$f='';
						if(isset($a['ma']) && is_array($a['ma']) && sizeof($a['ma']))
						{
							foreach($a['ma'] as $ma => $dummy)
								$f .= '('.$ma.')';
						}
						if(isset($a['mf']) && is_array($a['mf']) && sizeof($a['mf']))
						{
							foreach($a['mf'] as $mf => $dummy)
								$f .= '('.$mf.'f)';
						}

						$add[$pers]=$ek.$GLOBALS['_divstr'][0].$vk.$GLOBALS['_divstr'][0].$f;
					}
				}
			}
			if($es)
			{
				$csv .= $GLOBALS['_divstr'][1].'-1'.$GLOBALS['_divstr'][0].'&lt;d&gt;';
				if($sa)
					$csv .= $GLOBALS['_divstr'][0].'0'.$GLOBALS['_divstr'][0].'0'.$GLOBALS['_divstr'][0];
			}
			foreach($source as $k => $v)
			{
				if(strlen($csv))
					$csv .= $GLOBALS['_divstr'][1];
				$csv .= $k.$GLOBALS['_divstr'][0].$v;
				if(fxIsArray($add) && isset($add[$k]))
					$csv .= $GLOBALS['_divstr'][0].$add[$k];
				$sec++;
			}
//for($i=1; $i<=1000; $i++) { $csv .= $GLOBALS['_divstr'][1].(1000000+$i).$GLOBALS['_divstr'][0].'Addon #'.$i.' ['.md5($i).']'; $sec++; }
//for($i=1; $i<=1000; $i++) { $csv .= $GLOBALS['_divstr'][1].(1000000+$i).$GLOBALS['_divstr'][0].'<img src=./66/PER/4686_ico.png?1534482351 align=top width=13 height=13 style=\'border:1px solid #d2d2d2;border-radius:2px;\' />&nbsp;Addon #'.$i.' ['.md5($i).']'; $sec++; }
		}
	}

	if(!$ret || ($ret === 1))
		$csv='<div id="sel_'.$fname.'" style="display:none;">'.$csv.'</div><div id="sec_'.$fname.'" style="display:none;">'.$sec.'</div>';
//if($fname != 'dbgmode') echo('<b><u>sel_'.$fname.'</u></b><pre style="width:1024px;white-space:normal;">'.fxHtmlEncode($csv).'</pre>');
	if($ret)
		return $csv;
	echo($csv);
}

/**
 * Auswahl-Funktionen
 *
 * @param integer $a_return    - Optional parameter (default = 0):       ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()): ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Kunde'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Kunde($a_return=0, $a_arg_array=array(), $s_feldname='Kunde')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

	if(isset($override) && ($a_return != -1))
	{
		$a_array=$override;
		$flt=$GLOBALS[$s_feldname.'_flt'];
		if($flt)
			$GLOBALS['mskfilter'] |= $flt;
//echo('<b>Kunde 1:</b> '.$flt.' -- GLOBAL: '.$GLOBALS['mskfilter'].'<br />');
	}
	else
	{
		// Select-Argumente vorbereiten
		$order='';
		if(($GLOBALS['fxpglobals']['persdat']['role'] != 2) && ($GLOBALS['fxpglobals']['persdat']['personentyp'] == 620) && !$alle_kunden)	// Ansprechpartner eines Kunden angemeldet
			$where="p.personen_id=".$GLOBALS['fxpglobals']['persdat']['elter']." AND p.mandanten_id=".$Mandanten_ID;
		else
		{
			// Auftraggeber filtern (eingeschränkt), ausser in Filtertransaktion
			if($GLOBALS['fxpglobals']['tr'] != 223)
				$filter=trim(filter_person('', false, false, $s_feldname, false, false));
			else
				$filter='';
			$where="p.personentyp=276 AND p.mandanten_id=".$Mandanten_ID;
			if(($GLOBALS['fxpglobals']['persdat']['role'] != 2) && $pr_check && !$alle_kunden)	// Nur projektspezifische Kunden
			{
				$where .= " AND p.personen_id IN (SELECT kunde FROM projekte WHERE mandanten_id=".$Mandanten_ID;
				if($GLOBALS['fxpglobals']['person'] && ($GLOBALS['fxpglobals']['user'] > 1) && !$GLOBALS['fxpglobals']['persdat']['roles'][2])
					$where .= " AND projekt_id IN (".prights_get(0).")";
				$where .= ")";
			}
			if(strlen($filter))
			{
				$fnl=strtolower($s_feldname);
				$agid=0;
				if(fxIsArray($GLOBALS['maskenfeldwerte']) && isset($GLOBALS['maskenfeldwerte'][$fnl]))
					$agid=(int)$GLOBALS['maskenfeldwerte'][$fnl];
				$where .= ' AND ('.$filter;
				if($agid && ($GLOBALS['fxpglobals']['action'] != FXP_CREATE))
					$where .= ' OR p.personen_id='.$agid;
				$where .= ')';
//fxDebug($GLOBALS['maskenfeldwerte'][$fnl], '$maskenfeldwerte[\''.$fnl.'\']');
//fxDebug($GLOBALS['fxpglobals']['action'], 'action');
//fxDebug($filter, '$filter');
			}
			$order=" ".db_orderby('pname', 'vorname');
		}
		$s_name = "SELECT p.personen_id, p.pname, p.vorname, p.inaktiv FROM personen p WHERE ".$where.$order;
		$s_wert = 'p.personen_id';
		$s_feld = 'pname';

		// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
		$arg_array=argArray($s_name,$s_func,$s_feld,$s_wert,$s_feldname,$a_arg_array);
		$a_array=array();
		$z_array=db_values($s_name, '*');
//fxDebug($z_array, $s_name);
		if(fxIsArray($z_array))
		{
			foreach($z_array as $z)
			{
				$pid=(int)$z['personen_id'];
				$a_array[$pid]=trim($z['pname']);
				if(strlen(trim($z['vorname'])))
					$a_array[$pid] .= ', '.trim($z['vorname']);
			}

			// Adressen als Zusatzinfo ermitteln
			$sql  = "SELECT a.personen_id, a.ort, w.tabwert FROM adressen a, wertetab w";
			$sql .= " WHERE a.personen_id IN (SELECT p.personen_id FROM personen p WHERE ".$where.")";
			$sql .= " AND w.wertetabid=a.landkz AND w.id_sprache=".$GLOBALS['fxpglobals']['lang']." AND w.mandanten_id IN (0,".$Mandanten_ID.") AND a.mandanten_id=".$Mandanten_ID;
			$adr=db_values($sql, '*');
			if(is_array($adr))
			{
				$pids_done=array();
				foreach($adr as $a)
				{
					$pid=$a['personen_id'];
					if($pids_done[$pid])
						continue;

					$a_array[$pid] .= ' ('.$a['tabwert'];
					if(strlen(trim($a['ort'])))
						$a_array[$pid] .= '-'.trim($a['ort']);
					$a_array[$pid] .= ')';

					$pids_done[$pid]=true;
				}
			}

			// Inaktiv
			$inakt=' {'.get_text(1391, '20').'}';	// {inaktiv}
			foreach($z_array as $z)
			{
				$pid=(int)$z['personen_id'];
				if($z['inaktiv'])
					$a_array[(int)$z['personen_id']] .= $inakt;
			}
		}
	}

//fxDebug($a_array, $s_name);
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):             ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):       ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Kunde_alias'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Kunde_alias($a_return=0, $a_arg_array=array(), $s_feldname='Kunde_alias')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return auswahl_Kunde($a_return, $a_arg_array, $s_feldname);
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):                 ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):           ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Ansprechpartner'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Ansprechpartner($a_return=0, $a_arg_array=array(), $s_feldname='Ansprechpartner')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

	// Select-Argumente vorbereiten
	$s_name  = "SELECT p.personen_id, p.pname, p.vorname, p.inaktiv, pe.pname AS epname, pe.vorname AS evorname, pe.inaktiv AS einaktiv";
	$s_name .= " FROM (personen p LEFT JOIN personen pe ON p.elter=pe.personen_id AND p.mandanten_id=pe.mandanten_id)";
	$s_name .= " WHERE ";
	$where="p.mandanten_id=".$Mandanten_ID." AND p.personentyp=620 AND pe.mandanten_id=".$Mandanten_ID." AND pe.personentyp=276";
	if(($GLOBALS['prgclass'] < 40) && ($GLOBALS['fxpglobals']['persdat']['personentyp'] == 620))	// Ansprechpartner eines Auftraggebers angemeldet
	{
		$ag=(int)db_value("SELECT elter FROM personen WHERE personen_id=".$GLOBALS['fxpglobals']['person']);
		$where .= " AND pe.personen_id=".$ag;
	}
	else
	{
		$filter=filter_person('', false, false, $s_feldname, false, false);
		// Filter auch auf Elter erweitern
		$sp=strpos($filter,' OR ((');
		if($sp)
		{
			$sflt=substr($filter,0,$sp+5);
			$mflt=substr($filter,$sp+5,-2);
			$filter=$sflt.$mflt." AND ".str_replace('p.','pe.',$mflt)."))";
		}
		$fnl=strtolower($s_feldname);
		$pmid='';
		if(fxIsArray($GLOBALS['maskenfeldwerte']) && isset($GLOBALS['maskenfeldwerte'][$fnl]))
			$pmid=trim($GLOBALS['maskenfeldwerte'][$fnl]);
		if(isset($mvalues) && strlen($mvalues))
		{
			if(strlen($pmid))
				$pmid .= ',';
			$pmid .= $mvalues;
		}
		$where .= " AND (".$filter;
		if(strlen($pmid) && (($GLOBALS['fxpglobals']['action'] != FXP_CREATE) || ($GLOBALS['fxpglobals']['tr'] == 99)))
			$where.= " OR p.personen_id IN (".$pmid.")";
		$where .= ')';
//fxDebug($GLOBALS['maskenfeldwerte'][$fnl], '$maskenfeldwerte[\''.$fnl.'\']');
//fxDebug($GLOBALS['fxpglobals']['action'], 'action');
//fxDebug($filter, '$filter');
	}
	$s_name .= $where." ".db_orderby('pe.pname', 'pe.vorname', 'p.pname', 'p.vorname');
	$tmp=db_values($s_name,'*');
//fxDebug($tmp,$s_name);

	// Auswahl-Array in Auswahl-Feld umwandeln
	$a_array=false;
	if(sizeof($tmp))
	{
		$a_array=array();
		$inakt=' {'.get_text(1391, '20').'}';	// {inaktiv}
		foreach($tmp as $t)
		{
			$val=$t['epname'];
			if(strlen($t['evorname']))
				$val .= ', '.$t['evorname'];
			$val .= ' - '.$t['pname'];
			if(strlen($t['vorname']))
				$val .= ', '.$t['vorname'];
			if($t['inaktiv'] || $t['einaktiv'])
				$val .= $inakt;

			$a_array[(int)$t['personen_id']]=$val;
		}
	}
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):              ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):        ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Mandanten_ID'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Mandanten_ID($a_return=0, $a_arg_array=array(), $s_feldname='Mandanten_ID')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);

	if($GLOBALS['fxpglobals']['tr'] || $GLOBALS['fxpglobals']['client'])
	{
		// Nur Hauptmandant 1 sieht "alle" Mandanten
		$zusatz=" AND mandanten_id";
		if($GLOBALS['fxpglobals']['client'] > 1)
			$zusatz .= "=".$GLOBALS['fxpglobals']['client'];
		else
			$zusatz .= ">0";

		$from=" FROM personen WHERE personentyp=274".$zusatz;
		$mm=db_value("SELECT MAX(mandanten_id) AS mm".$from);
		if(!$mm)
			$mm=1;
		$lc=strlen($mm);
		$lz=str_repeat('0', $lc);

		$s_name="SELECT mandanten_id, pname, name_zusatz".$from." ORDER BY mandanten_id";
		$s_func='$selectwerte["pname"]=substr("'.$lz.'".(int)$selectwerte["mandanten_id"], -'.$lc.')." - ".$selectwerte["pname"]; if(strlen(trim($selectwerte["name_zusatz"]))) $selectwerte["pname"] .= " (".trim($selectwerte["name_zusatz"]).")";';
		$s_wert='mandanten_id';
		$s_feld='pname';

		// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
		$arg_array=argArray($s_name,$s_func,$s_feld,$s_wert,$s_feldname,$a_arg_array);
		$a_array=select_array($arg_array);
	}
	else
	{
		$a_array=array();

		// Mandanten
		$sql="SELECT mandanten_id, pname FROM personen WHERE personentyp=274 AND mandanten_id>0 ORDER BY mandanten_id";
		$mia=db_values($sql, '*');
		if(fxIsArray($mia))
		{
			// Benutzer
			$sql  = "SELECT b.mandanten_id, b.benutzer_id, b.personen_id, b.name_benutzer, p.pname, p.vorname, p.personentyp";
			$sql .= " FROM benutzer b, personen p WHERE p.personen_id=b.personen_id AND p.mandanten_id=b.mandanten_id ORDER BY b.mandanten_id, b.name_benutzer, b.personen_id";
			$bia=db_values($sql, '*');

			$mle=end($mia);
			$mln=strlen($mle['mandanten_id']);
			$nbl=str_repeat('&nbsp;', ($mln+1)*2);
			foreach($mia as $m)
			{
				$mk=$m['mandanten_id'];
				$a_array[$mk]=substr('000000000'.$mk, -$mln).':&nbsp;'.$m['pname'];

				if(fxIsArray($bia))
				{
					foreach($bia as $b)
					{
						$bm=$b['mandanten_id'];
						if($bm != $mk)
							continue;

						$bk=$mk.'-'.$b['personen_id'].'-'.$b['benutzer_id'];
						$a_array[$bk]=$nbl;
						if($b['benutzer_id'] == 1)
							$a_array[$bk] .= '*';
						$a_array[$bk] .= $b['name_benutzer'].'&nbsp;&nbsp;&nbsp;('.$b['pname'];
						if(strlen($b['vorname']))
							$a_array[$bk] .= ',&nbsp;'.$b['vorname'];
						$a_array[$bk] .= ')';
					}
				}
			}
		}
	}
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):               ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):         ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Projektleiter'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Projektleiter($a_return=0, $a_arg_array=array(), $s_feldname='Projektleiter')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

	$skill=1;
	if($s_feldname == 'Projektmanager')
		$skill=-2;

	// Alle Projektleiter ermitteln
	if($GLOBALS['fxpglobals']['tr'] != 223)	// ...Personen filtern (eingeschränkt), ausser in Filtertransaktion
		$filter=filter_person('', false, false, $s_feldname, false, false);
	else
		$filter='';
	$call  = "SELECT p.personen_id, p.pname, p.vorname, p.inaktiv, d.maeintrittsdatum, d.maaustrittsdatum";
	$call .= " FROM personen p, madaten d, ma_art_faehigkeiten m";
	$call .= " WHERE p.mandanten_id=".$Mandanten_ID." AND m.personen_id=p.personen_id AND m.mandanten_id=".$Mandanten_ID." AND m.mafaehigkeit_id=".$skill;
	$call .= " AND d.personen_id=p.personen_id AND d.mandanten_id=".$Mandanten_ID;
	if(strlen($filter))
	{
		$fnl=strtolower($s_feldname);
		$pmid='';
		if(fxIsArray($GLOBALS['maskenfeldwerte']) && isset($GLOBALS['maskenfeldwerte'][$fnl]))
			$pmid=trim($GLOBALS['maskenfeldwerte'][$fnl]);
		if(isset($mvalues) && strlen($mvalues))
		{
			if(strlen($pmid))
				$pmid .= ',';
			$pmid .= $mvalues;
		}
		$call .= " AND (".$filter;
		if(strlen($pmid) && (($GLOBALS['fxpglobals']['action'] != FXP_CREATE) || ($GLOBALS['fxpglobals']['tr'] == 99)))
			$call .= " OR p.personen_id IN (".$pmid.")";
		$call .= ')';
//fxDebug($GLOBALS['maskenfeldwerte'][$fnl], '$maskenfeldwerte[\''.$fnl.'\']');
//fxDebug($GLOBALS['fxpglobals']['action'], 'action');
//fxDebug($filter, '$filter');
	}
	$call .= " ORDER BY p.pname, p.vorname";
	$pa=db_values($call, '*');
//fxDebug($pa,$call);

	$a_array=array();
	if(fxIsArray($pa))
	{
		$inakt=' {'.get_text(1391, '20').'}';	// {inaktiv}
		foreach($pa as $p)
		{
			// Ist eine Person bereits einem Projekt zugeteilt?
			$sql="SELECT projekt_id FROM projekte WHERE ".strtolower($s_feldname)."=".$p['personen_id']." AND mandanten_id=".$Mandanten_ID;
			$mrp=(int)db_value($sql);

			if((substr($p['maeintrittsdatum'],0,8) > substr($GLOBALS['datetime'],0,8)) || (strlen($p['maaustrittsdatum']) && (substr($p['maaustrittsdatum'],0,8) < substr($GLOBALS['datetime'],0,8))))
				$name='<red>';
			else if($mrp)
				$name='<green>';
			else
				$name='';

			$name .= $p['pname'];
			if(strlen($p['vorname']))
				$name .= ', '.$p['vorname'];

			$name .= ' ('.get_entry($p['maeintrittsdatum'],'datum');
			if(strlen($p['maaustrittsdatum']))
				$name .= '-'.get_entry($p['maaustrittsdatum'],'datum');
			$name .= ')';

			if($p['inaktiv'])
				$name .= $inakt;

			$a_array[$p['personen_id']]=$name;
		}
	}
//fxDebug($a_array,'$a_array');
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):           ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):     ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Vertreter'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Vertreter($a_return=0, $a_arg_array=array(), $s_feldname='Vertreter')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return auswahl_Projektleiter($a_return, $a_arg_array, $s_feldname);
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):                  ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):            ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Mutterprojekt_ID'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Mutterprojekt_ID($a_return=0, $a_arg_array=array(), $s_feldname='Mutterprojekt_ID')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

	// $a_array direkt als Array übergeben
	$a_array=array();
	if(fxIsArray($arr))
		$a_array=$arr;
	else
	{
		// Select-Argumente vorbereiten
		$s_name  = "SELECT p.projekt_id, p.mutterprojekt_id, p.name_projekt, p.vorgangsnummer, p.ref_nummer, p.projektart";
		$s_name .= " FROM projekte p, zeitdaten z";
		$s_name .= " WHERE z.projekt_id=p.projekt_id AND z.zeitart=102 AND z.mandanten_id=".$Mandanten_ID." AND p.mandanten_id=".$Mandanten_ID;
		if(isset($with_subprojects) && $with_subprojects)
			$s_name .= " AND p.projektart IN (".FXP_PRJ_MAIN.",".FXP_PRJ_SUB.")";
		else
			$s_name .= " AND p.elter=0 AND p.projektart=".FXP_PRJ_MAIN;

		// Nur Förderprojekte?
		if(isset($fp) && $fp)
			$s_name .= " AND p.ref_nummer LIKE 'EP%'";

		// Wenn User nicht Geschäftsführer und kein Sysadmin => Rechteprüfung
		if($GLOBALS['fxpglobals']['person'] && ($GLOBALS['fxpglobals']['user'] > 1) && !$GLOBALS['fxpglobals']['persdat']['roles'][2])
		{
			if($GLOBALS['fxpglobals']['tr'] == 223)
				$s_name .= " AND p.projekt_id IN (SELECT p.mutterprojekt_id FROM projekte p WHERE p.mandanten_id=".$Mandanten_ID." AND p.projekt_id IN (".prights_get(0)."))";
			else
				$s_name .= " AND p.projekt_id IN (".prights_get(0).")";
		}

		// Ausgecheckte Projekte auflisten?
		if(prjValidPF())
		{
			$cops=prjGetCheckedOut();
//echo('$cops='.$cops.'<br />');
			if(strlen($cops))
				$s_name .= " AND p.mutterprojekt_id NOT IN (".$cops.")";
		}

		// Filter (ausser in Filter Transaktion 223)
		if($GLOBALS['fxpglobals']['tr'] != 223)
			$s_name .= filter_projekt('and', false, false, $s_feldname);

		$s_name .= " ".db_orderby('vorgangsnummer');
	}

	$s_wert='projekt_id';
	$s_feld='name_projekt';

	if(!isset($GLOBALS['selpopptype']))
		$GLOBALS['selpopptype']=array();
	$GLOBALS['selpopptype'][$s_feldname]='---';

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	if(!sizeof($a_array))
	{
		$arg_array=array('s_name'=>$s_name, 's_feld'=>$s_feld, 's_wert'=>$s_wert, 'a_name'=>$s_feldname, 'a_select'=>$a_arg_array['a_select'], 'a_change'=>$a_arg_array['a_change']);
		$arr=db_values($s_name,'*');
//fxDebug($arr,$s_name);
		if(fxIsArray($arr))
		{
			// Sortierung nach Vorgansnummer oder Name?
			$sort_vn=false;
			if(substr(strtolower($GLOBALS['fxpglobals']['settings']['zeit_projektnr']),0,1) == 'j')
				$sort_vn=true;
//fxDebug($sort_vn,'$sort_vn');

			// Prefixes ermitteln
			getAllPrefixes();
//fxDebug($GLOBALS['mp_prefixes'], '$GLOBALS[\'mp_prefixes\']');
			$prfx=trim($GLOBALS['fxpglobals']['settings']['vnprefix']);
			$hide_pf=false;
			if(substr(strtolower($GLOBALS['fxpglobals']['settings']['zeige_prefix']),0,1) == 'n')
				$hide_pf=true;
//fxDebug($hide_pf,'$hide_pf');

			// Alle Mutterprojekte
			$mpa=array();
			$mpc=array();
			foreach($arr as $ak => $a)
			{
				$mpid=$a['mutterprojekt_id'];
				$pn=no_null($a['vorgangsnummer']);
				$lv=floor(strlen($a['vorgangsnummer'])/5);
				if(!$lv)
				{
					if($sort_vn)
					{
						$sn=$a['vorgangsnummer'].'_'.$a['name_projekt'];
						$nm=$pn.' - '.$a['name_projekt'];
					}
					else
					{
						$sn=$a['name_projekt'].'_'.$a['vorgangsnummer'];
						$nm=$a['name_projekt'].' ['.$pn.']';
					}
					if(isset($fp) && $fp)
					{
						$sn=$a['ref_nummer'].'_'.$sn;
						$nm=$a['ref_nummer'].' '.$nm;
					}
					$mpa[str_replace(' ','_',strtolower($sn))]=array('sk'=>$ak, 'nm'=>$nm);
					$mpc[$mpid]=array('ks'=>$ak, 'mp'=>true);
				}
				else
				{
					$np=$a['name_projekt'];
					if(strlen($prfx) && $hide_pf && isset($GLOBALS['mp_prefixes'][$mpid]) && (substr($np,0,strlen($GLOBALS['mp_prefixes'][$mpid])) == $GLOBALS['mp_prefixes'][$mpid]))
						$np=trim(substr($np,strlen($GLOBALS['mp_prefixes'][$mpid])));
					$nm=str_repeat('&nbsp;',$lv*3).$pn.' - '.$np;
					if(!isset($mpc[$mpid]))
						$mpc[$mpid]=array('ks'=>$ak, 'mp'=>false);
				}

				$arr[$ak]['value']=$nm;
			}
//fxDebug($mpc,'$mpc');
//fxDebug($arr,'$arr');

			// Nicht direkt berechtigte Mutterprojekte trotzdem ermitteln und auflisten
			if(sizeof($mpc))
			{
				foreach($mpc as $mpid => $ma)
				{
					if(!$ma['mp'])
					{
						$sn=substr($arr[$ma['ks']]['vorgangsnummer'],0,4);
						$nm=db_value("SELECT name_projekt FROM projekte WHERE vorgangsnummer='".$sn."' AND mandanten_id=".$Mandanten_ID);
						if($sort_vn)
							$nm=no_null($sn).' - <i>'.$nm.'</i>';
						else
							$nm = '<i>'.$nm.'</i> ['.no_null($sn).']';
						$mpa[$sn]=array('sk'=>$ma['ks'], 'nm'=>$nm);
					}
				}
			}

			// Zuletzt gesetzte Checkboxen wieder herstellen
			$psps=array();
			if(fxIsArray($GLOBALS['fxptarray']) && isset($GLOBALS['fxptarray']['psps_'.$s_feldname]) && strlen($GLOBALS['fxptarray']['psps_'.$s_feldname]))
				$psps=explode('|',$GLOBALS['fxptarray']['psps_'.$s_feldname]);
//fxDebug($psps,'psps_'.$s_feldname);

			$mpc='-';
			$spc='-';
			$tsc='-';

			$a_array=array();
			if(sizeof($mpa))
			{
				ksort($mpa);
//fxDebug($mpa,'$mpa');
				foreach($mpa as $mk => $sa)
				{
					$sk=$sa['sk'];
					$nm=$sa['nm'];
					$mpid=$arr[$sk]['mutterprojekt_id'];
					if(isset($with_subprojects) && $with_subprojects && sizeof($a_array) && !isset($GLOBALS['app_lang']))
						$a_array[-$mpid]='<d>';
					$a_array[$mpid]=$nm.' {**}';
					$mpc='1';
					$k=$sk;
					if($arr[$sk]['projekt_id'] == $mpid)
						$k++;
					while(isset($arr[$k]) && ($arr[$k]['mutterprojekt_id'] == $mpid))
					{
						$a_array[$arr[$k]['projekt_id']]=$arr[$k]['value'].' {*}';
						$spc='1';
						$k++;
					}
				}
			}

			if($mpc != '-')
			{
				if(sizeof($psps) && isset($psps[0]))
					$mpc=$psps[0];
				$ptc++;
			}
			if($spc != '-')
			{
				if(sizeof($psps) && isset($psps[1]))
					$spc=$psps[1];
				$ptc++;
			}
			if($tsc != '-')
			{
				if(sizeof($psps) && isset($psps[2]))
					$tsc=$psps[2];
				$ptc++;
			}
//echo('<b>'.$s_feldname.'</b> -- $mpc='.$mpc.', $spc='.$spc.', $tsc='.$tsc.' -&gt; $ptc='.$ptc.'<hr />');
			if(!isset($GLOBALS['noselpopptype']) && ($ptc > 1))
				echo('<input id="selpopptype_'.$s_feldname.'" name="selpopptype_'.$s_feldname.'" type="hidden" value="'.$mpc.'|'.$spc.'|'.$tsc.'">');
		}
		else
			$a_array=false;
//$a_array=select_array($arg_array);
	}
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):            ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):      ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Projekt_ID'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Projekt_ID($a_return=0, $a_arg_array=array(), $s_feldname='Projekt_ID')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	//$dbbta=debug_backtrace(); $dbbtf=$dbbta[0]['file']; $dbbtl=$dbbta[0]['line']; echo('<b>auswahl_Projekt_ID</b> '.$dbbtf.' ('.$dbbtl.')<hr />');

	$_STA=array(0=>microtime_double());

	$use_structsel=false;
	if($use_structsel)
	{
		echo('<font class="lightgrey">SELECT Start:</font> <font class="s2">'.$_STA[0].'</font><br />');
		show_memory_usage(__LINE__, __FILE__, true);
	}

	$feld_projekt_array=$GLOBALS['fxptdata']['backup']['projekt_array'];

	// Zwischenspeichern der Projektkriterien NUR in "Projekte definieren"
	if($feld_projekt_array && ($GLOBALS['fxpglobals']['tr'] == 23))
		$auswahlkriterien=$feld_projekt_array;
	else
		$auswahlkriterien=$GLOBALS['fxpglobals']['filter'];

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];

	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];
//fxDebug($arg,'$arg');

	$disp_structure=false;

	// Sortierung
	$sort_vn=false;
	if(strtolower($GLOBALS['fxpglobals']['settings']['zeit_projektnr']) == 'ja')
		$sort_vn=true;
	// Ausnahme: Bei Zeitkontoänderungen (Transaktion 146) das Zielfeld immer nach Projektnummer sortieren
	if(($GLOBALS['fxpglobals']['tr'] == 146) && ($s_feldname == 'Projekt_ID'))
		$sort_vn=true;
//fxDebug($sort_vn,'$sort_vn');

	// Projekte anzeigen?
	$dmp='<d>';
	$dsp='<d>';
	if(isset($with_projects) && $with_projects)
	{
		$dmp=' {**}';
		$dsp=' {*}';
	}

	// Prefixes ermitteln
	getAllPrefixes();
//fxDebug($GLOBALS['mp_prefixes'], '$GLOBALS[\'mp_prefixes\']');

	// $s_name direkt als Array übergeben
	if($comp_array)
		$s_name=$comp_array;
	else if(isset($build))
	{
//fxDebug($arg,'$arg');
		$disp_structure=false;
		$verr=false;

		// Rechte
		// ...Nur berechtigte Projekte anklickbar?
		if(isset($only_click_allowed) && $only_click_allowed)
			$only_click_allowed=true;
		else
			$only_click_allowed=false;
		// ...Projekte
		$prj_rights="";
		if($GLOBALS['fxpglobals']['person'] && ($GLOBALS['fxpglobals']['user'] > 1) && !$GLOBALS['fxpglobals']['persdat']['roles'][2])
			$prj_rights=prights_get(0,0,true);
//echo('<b>$prj_rights</b>: &quot;'.$prj_rights.'&quot;<br />');
		// ...Personen
		$prs_rights_csv="";
		$ap=hasRightsForAllPersons();
		if(!$ap)
			$prs_rights_csv=get_persons();
//echo('<b>$prs_rights_csv</b>: &quot;'.$prs_rights_csv.'&quot;<hr size=1 />');

		// Filter-Kriterien
		// ...Projekte
		$prj_filter='';
		if($GLOBALS['fxpglobals']['tr'] != 114)
		{
			if(isset($use_actual) && $use_actual)
			{
				$GLOBALS['ignore_soll_beginn']=true;
				$GLOBALS['ignore_soll_ende']=true;
			}
			$prj_filter=filter_projekt('and',false,false, $s_feldname);
		}
//echo('<b>$prj_filter</b>: &quot;'.$prj_filter.'&quot;<br />');
		// ...Personen
		$prs_filter=filter_person('and',false,false,'',false);
//echo('<b>$prs_filter</b>: &quot;'.$prs_filter.'&quot;<hr size=1 />');

		// SQL-Kriterien
		// ...Ist-Daten statt Soll-Daten für den Zeitraum verwenden?
		$actual=false;
		$z_type=FXP_TE_PROJECT;
		$z_start="z.soll_beginn";
		$z_end="z.soll_ende";
		if(isset($use_actual) && $use_actual)
		{
			$actual=true;
			$z_type=FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_MATUSAGE.",".FXP_TE_TRAVELEXP.",".FXP_TE_TRAVELEXP_TIMEREC;
			$z_start="z.ist_beginn";
			$z_end="z.ist_ende";
		}
		// ...Zeitraum einschränken?
		$sql_date="";
		$prj_start='';
		if(isset($start_date) && strlen($start_date))
			$prj_start=validate_date($start_date, $verr, true).'000000';
		$prj_end='';
		if(isset($end_date) && strlen($end_date))
			$prj_end=validate_date($end_date, $verr, true).'240000';
		if(strlen($prj_start) && strlen($prj_end) && ($prj_end < $prj_start))
		{
			$swap=$prj_end;
			$prj_end=$prj_start;
			$prj_start=$swap;
		}
		if(strlen($prj_start) && strlen($prj_end))
			$sql_date=" AND (".$z_end.">='".$prj_start."' AND ".$z_start."<='".$prj_end."')";
		else if(strlen($prj_start))
			$sql_date=" AND ".$z_end.">='".$prj_start."'";
		else if(strlen($prj_end))
			$sql_date=" AND ".$z_start."<='".$prj_end."'";
//echo('<b>$sql_date</b>: &quot;'.$sql_date.'&quot;<br />');
		// ...Auftraggeber einschränken?
		$sql_customers="";
		if(isset($customers) && $customers)
		{
			if($actual)
				$sql_customers=" AND z.zkunde IN (".$customers.")";
			else
				$sql_customers=" AND p.kunde IN (".$customers.")";
		}
//echo('<b>$sql_customers</b>: &quot;'.$sql_customers.'&quot;<br />');
		// ...Personen einschränken?
		$sql_persons="";
		if(isset($persons) && $persons)
			$sql_persons=" AND z.personen_id IN (".$persons.")";
//echo('<b>$sql_persons</b>: &quot;'.$sql_persons.'&quot;<hr size=1 />');

		// Ausgewählte Projekte aus der Datenbank auslesen
		$sql  = "SELECT DISTINCT p.projekt_id, p.mutterprojekt_id, p.vorgangsnummer";
		$sql .= " FROM projekte p, zeitdaten z";
		$sql .= " WHERE p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
		if(strlen($prj_filter))
			$sql .= $prj_filter;
		if(strlen($prj_rights))
			$sql .= " AND p.projekt_id IN (".$prj_rights.")";
		$sql .= " AND z.projekt_id=p.projekt_id AND z.zeitart IN (".$z_type.") AND z.mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$sql .= $sql_date.$sql_customers.$sql_persons;
		if($actual)
		{
			if(strlen($prs_rights_csv))
				$sql .= " AND z.personen_id IN (".$prs_rights_csv.")";
			if(strlen($prs_filter))
				$sql .= " AND z.personen_id IN (SELECT p.personen_id FROM personen p WHERE p.mandanten_id=".$GLOBALS['fxpglobals']['client'].$prs_filter.")";
		}
		if(isset($sql_addon) && strlen($sql_addon))
			$sql .= " ".$sql_addon;
//echo('<b>$sql</b>: &quot;'.$sql.'&quot;<hr size=1 />');
		$tmp=db_values($sql,'*');
//fxDebug($tmp, $sql);
		// ...Aktuell berechtigte Projekte und Mutterprojekte
		$pra=array();
		$mra=array();
		if(fxIsArray($tmp))
		{
			foreach($tmp as $k => $t)
			{
				$pra[$t['vorgangsnummer']]=(int)$t['projekt_id'];
				$mra[substr($t['vorgangsnummer'],0,4)]=(int)$t['mutterprojekt_id'];
			}
		}
		$prc=array_to_csv($pra,false);
//fxDebug($pra,'$pra: $prc='.$prc);
		$mrc=array_to_csv($mra,false);
//fxDebug($mra,'$mra: $mrc='.$mrc);
		// ...Fehlende übergeordnete Projekte
		$era=array();
		if(sizeof($pra))
		{
			foreach($pra as $n => $i)
			{
				$e=substr($n,0,-5);
				while(strlen($e) > 4)
				{
					if(!isset($era[$e]) && !isset($pra[$e]))
						$era[$e]=" OR vorgangsnummer='".$e."'";
					$e=substr($e,0,-5);
				}
			}
		}
		$erc="";
		if(sizeof($era))
		{
			foreach($era as $e)
				$erc .= $e;
		}
//fxDebug($era,'$era: $erc='.$erc);

		$sqls="SELECT projekt_id, mutterprojekt_id, projektart, vorgangsnummer, name_projekt FROM projekte WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND ";

		// Alle benötigten Mutterprojekte aus der Datenbank auslesen
		$ma=false;
		if(strlen($mrc))
		{
			$sql=$sqls."projekt_id IN (".$mrc.") ORDER BY ";
			// ...nach Projektname sortieren?
			if(!$sort_vn)
				$sql .= "name_projekt, ";
			$sql .= "vorgangsnummer";
			$ma=db_values($sql,'*');
//fxDebug($ma, $sql);
		}

		// Alle benötigten Teilprojekte und Aufgaben aus der Datenbank auslesen
		$pa=false;
		if(strlen($prc))
		{
			$sql=$sqls."(projekt_id IN (".$prc.")".$erc.") ORDER BY vorgangsnummer";
			$pa=db_values($sql,'*');
//fxDebug($pa, $sql);
		}

		// Auswahl-Array erstellen
		$mpc='-';
		$spc='-';
		$tsc='-';
		$override=array();
		// ...[Alle Projekte] oder [Unbekannt] anzeigen? Wenn ja, ganz oben in der Liste (beides noch nicht möglich, da ID=0)
		if(isset($with_all) && $with_all && strlen($mrc))
			$override[0]=meldung(1077,false);	// Alle Projekte
		else if(isset($with_unknown) && $with_unknown)
			$override[0]=meldung(1449,false);	// [Unbekannt]
		// ...Projekte
		if(strlen($mrc))
		{
			foreach($ma as $m)
			{
				$id=(int)$m['projekt_id'];
				$mn=$m['vorgangsnummer'];
				if($sort_vn)
					$st=no_null($mn).' - '.$m['name_projekt'];
				else
					$st=$m['name_projekt'].' ['.no_null($mn).']';

				if((sizeof($override) > 0) && !isset($GLOBALS['app_lang']))
					$override[-$id]='<d>';
				$override[$id]=$st.$dmp;
				if($only_click_allowed && !isset($pra[$mn]))
					$override[$id] .= '<d>';
				$mpc='1';
//echo('<b>$build</b> $id='.$id.': ['.fxHtmlEncode($override[$id]).']<br />');

				// ...untergeordnete Projekte/Aufgaben des Mutterprojekts
				foreach($pa as $p)
				{
					$im=$p['vorgangsnummer'];
					$il=strlen($im);
					$mp=substr($im,0,4);
					if(($mp == $mn) && ($il > 4))
					{
						$id=(int)$p['projekt_id'];
						$mi=(int)$p['mutterprojekt_id'];
						$lv=floor($il/5);
						$spacer='';
						if(!isset($GLOBALS['app_lang']))
							$spacer=str_repeat('&nbsp;',$lv*3);

						// Prefix ausblenden?
						$nm=$p['name_projekt'];
						if((strtoupper(substr($GLOBALS['fxpglobals']['settings']['zeige_prefix'], 0, 1)) == 'N') && sizeof($GLOBALS['mp_prefixes']) && isset($GLOBALS['mp_prefixes'][$mi]))
						{
							$pf=$GLOBALS['mp_prefixes'][$mi];
							$pl=strlen($pf);
							if(substr($nm,0,$pl) == $pf)
								$nm=trim(substr($nm,$pl));
						}

						$override[$id]=$spacer.no_null($im).' - '.$nm;
						if($p['projektart'] < FXP_PRJ_TASK)
						{
							$override[$id] .= $dsp;
							$spc='1';
						}
						else
							$tsc='1';
						if($only_click_allowed && !isset($pra[$im]))
							$override[$id] .= '<d>';
//echo('<b>$build</b> $id='.$id.': ['.fxHtmlEncode($override[$id]).']<br />');
					}
				}
			}
		}

		if(!sizeof($override))
			$override[0]=meldung(794,true);	// - leer -

		// Zuletzt gesetzte ProjektArtPopupCheckboxen wieder herstellen
		$psps=array();
		if(fxIsArray($GLOBALS['fxptarray']) && isset($GLOBALS['fxptarray']['psps_'.$s_feldname]) && strlen($GLOBALS['fxptarray']['psps_'.$s_feldname]))
			$psps=explode('|',$GLOBALS['fxptarray']['psps_'.$s_feldname]);
//fxDebug($psps,'psps_'.$s_feldname);
		if((sizeof($psps) > 0) && ($mpc != '-'))
			$mpc=$psps[0];
		if((sizeof($psps) > 1) && ($spc != '-'))
			$spc=$psps[1];
		if((sizeof($psps) > 2) && ($tsc != '-'))
			$tsc=$psps[2];
		echo('<input id="selpopptype_'.$s_feldname.'" name="selpopptype_'.$s_feldname.'" type="hidden" value="'.$mpc.'|'.$spc.'|'.$tsc.'">');
//fxDebug($override,'$override');
	}
	else if(isset($timereg))
	{
		$override=array();

		if(isset($with_unknown) && $with_unknown)
			$override[0]=meldung(1449,false);	// [Unbekannt]

		if(isset($pdatum) && strlen($pdatum))
			$pdatum=substr($pdatum,0,8).'240000';
		else
		{
			$pdatum=substr($GLOBALS['datetime'],0,8).'240000';
			if(strtolower($GLOBALS['fxpglobals']['settings']['ze_alle_aktiv']) == 'ja')											// Soll die Zeit- und Materialverbrauchserfassung generell auf alle aktiven Aufgaben ohne Startzeitbegrenzung erlaubt sein?
				$pdatum='99999999240000';
			else if(isset($bis) && strlen($bis) && (strtolower($GLOBALS['fxpglobals']['settings']['zeit_zukunft']) == 'ja'))	// Zeit- und Materialverbrauchserfassung für ein zukünftiges Datum erlauben?
				$pdatum=max($pdatum, substr($bis,0,8).'240000');
		}
//fxDebug($pdatum,'$pdatum: $bis='.$bis);

		if(isset($status) && strlen($status))
			$pstat=$status;
		else
			$pstat=FXP_PS_ACTIVE;

		// Aufgaben ermitteln
		$GLOBALS['ignore_soll_beginn']=true;
		$GLOBALS['ignore_projektart']=true;
		$GLOBALS['ignore_projekt_status']=true;
		$GLOBALS['ignore_kategorie_id']=true;

		$GLOBALS['aufids']='';
		$pflt=filter_projekt('and',false,false);

		if(isset($person) && $person)
		{
			$sqlz  = " FROM projekte p, maplanung m, zeitdaten z";
			$sqlz .= " WHERE p.projekt_id=m.projekt_id AND p.mandanten_id=m.mandanten_id AND p.projekt_id=z.projekt_id AND p.mandanten_id=z.mandanten_id";
			$sqlz .= " AND z.zeitart=102 AND m.personen_id=".$person." AND m.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND p.projektart=".FXP_PRJ_TASK." AND (";
			if(isset($project) && $project && ($GLOBALS['Dummy'] == FXP_CHANGE)) // auch bereits abgeschl. Projekt im "Ändern"-Modus anzeigen
				$sqlz .= "p.projekt_id=".$project." OR (";
			$sqlz .= "p.projekt_status IN (".$pstat.") AND ((z.soll_beginn<='".$pdatum."' AND (p.kategorie_id IS NULL OR p.kategorie_id<>1921)) OR (z.soll_ende<'".substr($GLOBALS['datetime'],0,8)."240000' AND p.kategorie_id=1921))";
			$sqlz .= " AND p.projekt_id NOT IN (SELECT projekt_id FROM termine WHERE personen_id=".$person." AND zeit_id IS NOT NULL AND zeit_id<>0 AND projekt_id IS NOT NULL AND projekt_id>0 AND (erledigt IS NULL OR erledigt=0 OR bestaetigung_anmerk LIKE '0-%'))";
			$sqlz .= $pflt;
			if(isset($project) && $project && ($GLOBALS['Dummy'] == FXP_CHANGE))
				$sqlz .= ")";
			$sqlz .= ")";
		}
		else if(isset($alltasks) && $alltasks)
		{
			$t=0;
			if(isset($akt))
				$t=(int)$akt;

			$sqlz  = " FROM projekte p, zeitdaten z";
			$sqlz .= " WHERE z.projekt_id=p.projekt_id AND z.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND z.zeitart=102 AND (";
			if($t)
				$sqlz .= "p.projekt_id=".$t." OR (";
			$sqlz .= "p.projektart=".FXP_PRJ_TASK." AND p.projekt_status IN (".$pstat.") AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$sqlz .= $pflt;
			if($t)
				$sqlz .= ")";
			$sqlz .= ")";
		}
		else
		{
			$sqlz  = " FROM projekte p, zeitdaten z";
			$sqlz .= " WHERE z.projekt_id=p.projekt_id AND z.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND z.zeitart=102";
			$sqlz .= " AND p.projektart=".FXP_PRJ_TASK." AND p.projekt_status IN (".$pstat.") AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$sqlz .= " AND ((z.soll_beginn<='".$pdatum."' AND (p.kategorie_id IS NULL OR p.kategorie_id<>1921)) OR (z.soll_ende<'".substr($GLOBALS['datetime'],0,8)."240000' AND p.kategorie_id=1921))";
			$sqlz .= " AND p.projekt_id NOT IN (SELECT projekt_id FROM termine WHERE personen_id=".$GLOBALS['fxpglobals']['person']." AND zeit_id IS NOT NULL AND zeit_id<>0";
			$sqlz .= " AND projekt_id IS NOT NULL AND projekt_id>0 AND (erledigt IS NULL OR erledigt=0 OR bestaetigung_anmerk LIKE '0-%'))";
			$sqlz .= $pflt;
		}

		$sql="SELECT p.projekt_id, p.mutterprojekt_id, p.name_projekt, p.vorgangsnummer".$sqlz." ORDER BY p.vorgangsnummer";
		$tmp=db_values($sql,'*');
//fxDebug($tmp, $sql);

		$tsk_array=array();
		if(fxIsArray($tmp))
		{
			$GLOBALS['aufids']="SELECT p.projekt_id".$sqlz;

			foreach($tmp as $t)
				$tsk_array[$t['vorgangsnummer']]=$t;
		}
//fxDebug($tsk_array, $sql);

		if(sizeof($tsk_array))
		{
			// Kostenrechnung aktiv, dann alle Leistungsarten pro Aufgabe anzeigen
			$la_array=array();
			if(function_exists('fxsppCheckEBTR') && fxsppCheckEBTR(241) && (substr(strtolower($GLOBALS['fxpglobals']['settings']['kr_aktiv']), 0, 1) == 'j') && (!isset($nocost) || !$nocost))
			{
				$kids='';

				// Alle zu den Aufgaben (904) zugeordneten Kostenarten (902) im gültigen Zeitraum
				$sqlz  = " FROM kostenzuordnung";
				$sqlz .= " WHERE kostenstellenart=904 AND kostendefinition=902 AND gueltig_von<='".substr($GLOBALS['datetime'],0,8)."240000'";
				$sqlz .= " AND gueltig_bis>='".substr($GLOBALS['datetime'],0,8)."000000' AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND id IN (".$GLOBALS['aufids'].")";

				$sql="SELECT kostenstelle_id, id".$sqlz;
				$tmp=db_values($sql,'*');
//fxDebug($tmp, $sql);
				if(fxIsArray($tmp))
				{
					$kids="SELECT kostenstelle_id".$sqlz;

					foreach($tmp as $t)
					{
						if(!isset($la_array[$t['id']]))
							$la_array[$t['id']]=array();
						$la_array[$t['id']]['kart']=$t['kostenstelle_id'];
					}
				}

				// Alle zu den Kostenarten zugeordneten Leistungsarten (903) im aktuellen Zeitraum ermitteln
				if(strlen($kids))
				{
					$sql  = "SELECT s.kostenstelle_id, s.kostennummer, s.kostenbezeichnung, s.produktiv, b.elter";
					$sql .= " FROM kostenstellen s, kostenbeziehungen b";
					$sql .= " WHERE s.kostendefinition=903 AND s.gueltig_von<='".substr($GLOBALS['datetime'],0,8)."240000'";
					$sql .= " AND s.gueltig_bis>='".substr($GLOBALS['datetime'],0,8)."000000' AND s.mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$sql .= " AND s.id_sprache=".$GLOBALS['fxpglobals']['lang']." AND s.kostenstelle_id=b.kostenstelle_id AND s.mandanten_id=b.mandanten_id AND b.elter IN (".$kids.")";
					$sql .= " ORDER BY s.kostennummer";
					$tmp=db_values($sql,'*');
//fxDebug($tmp, $sql);
					if(fxIsArray($tmp))
					{
						$ohne='['.meldung(388,false).']';	// [Ohne]

						foreach($tmp as $t)
						{
							foreach($la_array as $aid => $l)
							{
								if(($l['kart'] == $t['elter']) && !$la_array[$aid]['lart'][$t['kostenstelle_id']])
								{
									if(isset($emptycost) && $emptycost && !fxIsArray($la_array[$aid]['lart']))
										$la_array[$aid]['lart'][0]=$ohne;

									$lart=$t['kostennummer'];
									if($t['kostenbezeichnung'])
										$lart .= ': '.$t['kostenbezeichnung'];
									if($t['produktiv'])
										$lart .= ' {+}';
									$la_array[$aid]['lart'][$t['kostenstelle_id']]=$lart;
								}
							}
						}
					}
				}
//fxDebug($la_array, '$la_array');
			}

			// Muss eine Leistungsart angegeben werden?
			if(strtolower($GLOBALS['fxpglobals']['settings']['kr_lamuss']) == 'ja')
				$zz='=> ';
			else
				$zz='-> ';

			// Alle Mutterprojekte ermitteln
			$mpa=array();
			$mpt=array();
			$mpc="";
			foreach($tsk_array as $t)
			{
				if(!isset($mpt[$t['mutterprojekt_id']]))
				{
					$mpt[$t['mutterprojekt_id']]=true;
					if(strlen($mpc))
						$mpc .= ',';
					$mpc .= $t['mutterprojekt_id'];
				}
			}
			$sql  = "SELECT projekt_id, vorgangsnummer, name_projekt FROM projekte";
			$sql .= " WHERE projekt_id IN (".$mpc.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$sql .= " ORDER BY ";
			// ...nach Projektname sortieren?
			if(!$sort_vn)
				$sql .= "name_projekt, ";
			$sql .= "vorgangsnummer";
			$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
			if(fxIsArray($tmp))
			{
				foreach($tmp as $t)
					$mpa[$t['projekt_id']]=$t;
			}
//fxDebug($mpa,'$mpa');

			// Alle Teilprojekte ermitteln
			$spa=array();
			$sql  = "SELECT projekt_id, mutterprojekt_id, vorgangsnummer, name_projekt FROM projekte";
			$sql .= " WHERE mutterprojekt_id IN (".$mpc.") AND projektart=".FXP_PRJ_SUB." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$sql .= " ORDER BY vorgangsnummer";
			$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
			if(fxIsArray($tmp))
			{
				foreach($tmp as $t)
					$spa[$t['vorgangsnummer']]=$t;
			}
//fxDebug($spa,'$spa');

			$sa=array();
			$dmp='<d>';
			$dsp='<d>';
			if(isset($with_projects) && $with_projects)
			{
				$dmp=' {**}';
				$dsp=' {*}';
			}
			// Circle through all master projects
			$mpcnt=0;
			$pds=$s_feldname.'_ds';
			if(isset($GLOBALS[$pds]) && $GLOBALS[$pds])
				$mpcnt++;
			foreach($mpa as $mpid => $mpd)
			{
//echo(__LINE__.': $mpcnt='.$mpcnt.': $mpid='.$pid.', $mpd='.$mpd.'<br />');
				if(($mpcnt || isset($override[0])) && !isset($GLOBALS['app_lang']))
					$override[-$mpid]='<d>';

				$mpid=(int)$mpid;
				$mpno=$mpd['vorgangsnummer'];
				if(!isset($GLOBALS['app_lang']))
				{
					if(!$sort_vn)
						$override[$mpid]=$mpd['name_projekt'].' ['.no_null($mpno).']'.$dmp;
					else
						$override[$mpid]=no_null($mpno).' - '.$mpd['name_projekt'].$dmp;
				}
//echo('<b>$timereg</b> $mpid='.$mpid.': ['.fxHtmlEncode($override[$mpid]).']<br />');

				foreach($tsk_array as $tsno => $tsd)
				{
					if(substr($tsno,0,4) === $mpno)
					{
						$tsid=(int)$tsd['projekt_id'];
						$tslv=floor(strlen($tsno)/5);

						// Add all sub projects inbetween
						for($sc=1; $sc<$tslv; $sc++)
						{
							$spno=substr($tsno,0,4+$sc*5);
							if(!isset($sa[$spno]))
							{
								$sa[$spno]=true;
								$splv=floor(strlen($spno)/5);
								if(!isset($GLOBALS['app_lang']))
								{
									$spid=(int)$spa[$spno]['projekt_id'];
									// Prefix ausblenden?
									$mi=(int)$spa[$spno]['mutterprojekt_id'];
									$nm=$spa[$spno]['name_projekt'];
									if((strtoupper(substr($GLOBALS['fxpglobals']['settings']['zeige_prefix'], 0, 1)) == 'N') && sizeof($GLOBALS['mp_prefixes']) && isset($GLOBALS['mp_prefixes'][$mi]))
									{
										$pf=$GLOBALS['mp_prefixes'][$mi];
										$pl=strlen($pf);
										if(substr($nm,0,$pl) == $pf)
											$nm=trim(substr($nm,$pl));
									}
									$override[$spid]=str_repeat('&nbsp;',$splv*3).no_null($spno).' - '.$nm.$dsp;
//echo('<b>$timereg</b> $spid='.$spid.': ['.fxHtmlEncode($override[$spid]).']<br />');
								}
							}
						}

						// Add task
						$spacer='';
						if(!isset($GLOBALS['app_lang']))
							$spacer=str_repeat('&nbsp;',$tslv*3);
						// Prefix ausblenden?
						$mi=(int)$tsd['mutterprojekt_id'];
						$nm=$tsd['name_projekt'];
/*
						if((strtoupper(substr($GLOBALS['fxpglobals']['settings']['zeige_prefix'], 0, 1)) == 'N') && sizeof($GLOBALS['mp_prefixes']) && isset($GLOBALS['mp_prefixes'][$mi]))
						{
							$pf=$GLOBALS['mp_prefixes'][$mi];
							$pl=strlen($pf);
							if(substr($nm,0,$pl) == $pf)
								$nm=trim(substr($nm,$pl));
						}
*/
						$override[$tsid]=$spacer.no_null($tsno).' - '.$nm;
//echo('<b>$timereg</b> $tsid='.$tsid.': ['.fxHtmlEncode($override[$tsid]).']<br />');

						// Add all possible activities
						if(sizeof($la_array) && is_array($la_array[$tsid]['lart']))
						{
							foreach($la_array[$tsid]['lart'] as $lid => $lnr)
							{
								$spacer='';
								if(!isset($GLOBALS['app_lang']))
								{
									if($lid)
										$lcol='<green>';
									else
										$lcol='<red>';
									$spacer=str_repeat('&nbsp;',($tslv+1)*3).$lcol;
								}
								$override[$tsid.'_'.$lid]=$spacer.$zz.$lnr;
							}
						}
					}
				}
				$mpcnt++;
			}
		}
//fxDebug($override,'$override');
	}
	else if(!fxIsArray($override) || ($a_return == -1))
	{
		// Rechte
		$rechte_sql="";
		if($GLOBALS['fxpglobals']['person'] && ($GLOBALS['fxpglobals']['user'] > 1) && !$GLOBALS['fxpglobals']['persdat']['roles'][2])
			$rechte_sql=prights_get(0);

		// Select-Argumente vorbereiten
		// Zeitart
		if($zeitart)
			$zart=" AND z.zeitart=".$zeitart;
		else
			$zart=" AND z.zeitart=102";

		// Für Rominas Qualitätsmanagement-Programm werden auch die Soll-Zeiträume eingeblendet
		$s_select="SELECT p.projekt_id";
		if($a_return != -1)
		{
			$s_select .= ", p.mutterprojekt_id, p.name_projekt, p.vorgangsnummer, p.projektart, p.projektmanager, p.projektleiter, p.vertreter, p.maart_id, p.mafaehigkeit_id";
			$s_select .= ", m.name_projekt AS name_mutterprojekt, m.vorgangsnummer AS mutterprojektnummer";
			$s_select .= ", z.soll_beginn, z.soll_ende";
		}

		$s_from   = "FROM projekte p, projekte m, zeitdaten z";

		$s_where  = "WHERE z.projekt_id=p.projekt_id AND m.projekt_id=p.mutterprojekt_id";
		$s_where .= " AND p.mandanten_id=".$Mandanten_ID." AND m.mandanten_id=".$Mandanten_ID." AND z.mandanten_id=".$Mandanten_ID;
		$s_where .= $zart;
		if(strlen($rechte_sql))
			$s_where .= " AND p.projekt_id IN (".$rechte_sql.")";

		// Filter
		$s_and=filter_projekt('and', false, false, $s_feldname);
		$s_or='';

		// Projektfeld: Aktuell gewähltes Projekt immer in Liste anzeigen
		if(isset($rp_projektid) && $rp_projektid)
			$s_or .= " OR p.projekt_id=".(int)$rp_projektid;
		else if($auswahlkriterien['Backup']['Projekt_IDopen'] && ($s_feldname == 'Projekt_IDopen'))
			$s_or .= " OR p.projekt_id=".$auswahlkriterien['Backup']['Projekt_IDopen'];

		// Projektarten ausschliessen
		if($mp_only)
			$s_and .= " AND p.projektart=".FXP_PRJ_MAIN;
		else if(($GLOBALS['fxpglobals']['tr'] == 163) || $task_only)
			$s_and .= " AND p.projektart=".FXP_PRJ_TASK;	// Qualitätsmanagement => nur Aufgaben
		else
		{
			$disp_structure=true;
			 if(($GLOBALS['fxpglobals']['filter']['aufgaben'] || !$GLOBALS['fxpglobals']['setfilter'] || $GLOBALS['fxpglobals']['filter']['fprojekt_inaktiv'] || $with_tasks) && !$projects_only)
				$s_and .= " AND p.projektart>0";	// Every project type is allowed
			else
				$s_and .= " AND p.projektart<=".FXP_PRJ_SUB;
		}
		// AG, PL + PM einschränken, falls nicht GF
		if(!$GLOBALS['fxpglobals']['persdat']['roles'][2])
		{
			// Projektleiter
			if($auswahlkriterien['pl'])
				$rp_projektleiter=$auswahlkriterien['pl'];
			if(strlen($rp_projektleiter))
				$s_and .= " AND (p.projektleiter=".$rp_projektleiter." OR p.vertreter=".$rp_projektleiter." OR p.projektmanager=".$rp_projektleiter.")";

			// Projektmanager
			if(strlen($rp_projektmanager))
				$s_and .= " AND p.projektmanager=".$rp_projektmanager;

			// Auftraggeber
			if(strlen($rp_kunde))
				$s_and .= " AND p.kunde=".$rp_kunde;

			// Auftraggeber-Ansprechpartner
			if(strlen($rp_ansprechpartner))
				$s_and .= " AND p.ansprechpartner=".$rp_ansprechpartner;
		}

		// Projektstatus
		if(strlen($rp_projektstatus))
			$s_and .= " AND p.projekt_status IN (".$rp_projektstatus.")";

		// Ausgecheckte Projekte auflisten?
		if(prjValidPF())
		{
			$cops=prjGetCheckedOut();
//echo('$cops='.$cops.'<br />');
			if(strlen($cops))
				$s_and .= " AND p.mutterprojekt_id NOT IN (".$cops.")";
		}

		// Nur für "Projekte"
		if(($s_feldname == 'Abhaengigkeit') && (($auswahlkriterien['Backup']['Abhaengigkeit'] == 0) || $auswahlkriterien['Backup']['Abhaengigkeit']))
		{
			// Abhängigkeitsfeld: geöffnetes Projekt darf nicht zur Auswahl stehen
			if($auswahlkriterien['Backup']['Projekt_IDopen'])
				$s_and .= " AND p.projekt_id NOT IN (".inferior_vn($auswahlkriterien['Backup']['Projekt_IDopen']).")";
			// Abhängigkeitsfeld: Aktuellen Vorgänger immer in Liste anzeigen
			if($auswahlkriterien['Backup']['Abhaengigkeit'])
				$s_or .= " OR p.projekt_id=".$auswahlkriterien['Backup']['Abhaengigkeit'];
		}

		// Für "Rest" (irgendwann Projekte auf diesen Teil umstellen!)
		if($custom['Abhaengigkeit'] && ($s_feldname == 'Abhaengigkeit'))
		{
			// Abhängigkeitsfeld: geöffnetes Projekt darf nicht zur Auswahl stehen
			if($custom['Abhaengigkeit']['exclude'])
				$s_and .= " AND p.projekt_id NOT IN (".inferior_vn($custom['Abhaengigkeit']['exclude']).")";
			// Abhängigkeitsfeld: Aktuellen Vorgänger immer in Liste anzeigen
			if($custom['Abhaengigkeit']['include'])
				$s_or .= " OR p.projekt_id=".$custom['Abhaengigkeit']['include'];
		}

		// Sorgt dafür, dass aktuell gewähltes Projekt immer in Auswahl erscheint
		if(strlen($akt))
			$s_or .= " OR p.projekt_id IN (".$akt.")";

		// Zusatzbedingungen korrekt zum SQL dazufügen
		if(strlen($s_and))
		{
			if(strlen($s_or))
				$s_where .= " AND ((".substr($s_and, 5).")".$s_or.")";
			else
				$s_where .= $s_and;
		}
		else if(strlen($s_or))
			$s_where .= " OR (".substr($s_or, 4).")";

		$s_name=$s_select." ".$s_from." ".$s_where;

		if($a_return == -1)
			return $s_name;

		// nach Vorgangsnummer sortieren
		if($sort_vn)
		{
			$s_order=db_orderby('p.vorgangsnummer');
			// Prefix ausblenden?
			if((strtoupper(substr($GLOBALS['fxpglobals']['settings']['zeige_prefix'], 0, 1)) == 'N') && sizeof($GLOBALS['mp_prefixes']))
				$s_func="\$np=\$selectwerte['name_projekt']; if((\$selectwerte['projektart'] > FXP_PRJ_MAIN) && isset(\$GLOBALS['mp_prefixes'][\$selectwerte['mutterprojekt_id']])) { \$plen=strlen(\$GLOBALS['mp_prefixes'][\$selectwerte['mutterprojekt_id']]); if(substr(\$np, 0, \$plen) == \$GLOBALS['mp_prefixes'][\$selectwerte['mutterprojekt_id']]) \$np=trim(substr(\$np, \$plen)); } \$selectwerte['name_projekt']=no_null(\$selectwerte['vorgangsnummer']).' - '.\$np;";
			else
				$s_func='$selectwerte["name_projekt"]=no_null($selectwerte["vorgangsnummer"])." - ".$selectwerte["name_projekt"];';
			$s_func .= " if(\$selectwerte['projektart'] == FXP_PRJ_MAIN) \$selectwerte['name_projekt'] .= ' {**}'; else if(\$selectwerte['projektart'] == FXP_PRJ_SUB) \$selectwerte['name_projekt'] .= ' {*}';";
		}
		// nach Vorgangsname sortieren mit aktiviertem Prefix
		else if(strlen(trim($GLOBALS['fxpglobals']['settings']['vnprefix'])) && !$task_only)
		{
			$s_order=db_orderby('m.name_projekt', 'm.vorgangsnummer', 'p.vorgangsnummer');
			// Prefix ausblenden?
			if((strtoupper(substr($GLOBALS['fxpglobals']['settings']['zeige_prefix'], 0, 1)) == 'N') && sizeof($GLOBALS['mp_prefixes']))
				$s_func="if(\$selectwerte['projektart'] == FXP_PRJ_MAIN) \$selectwerte['name_projekt']='* '.\$selectwerte['name_projekt'].' ['.no_null(\$selectwerte['vorgangsnummer']).'] {**}'; else { \$np=\$selectwerte['name_projekt']; if(isset(\$GLOBALS['mp_prefixes'][\$selectwerte['mutterprojekt_id']])) { \$plen=strlen(\$GLOBALS['mp_prefixes'][\$selectwerte['mutterprojekt_id']]); if(substr(\$np, 0, \$plen) == \$GLOBALS['mp_prefixes'][\$selectwerte['mutterprojekt_id']]) \$np=trim(substr(\$np, \$plen)); } \$selectwerte['name_projekt']='&nbsp;&nbsp;'.no_null(\$selectwerte['vorgangsnummer']).' - '.\$np;";
			else
				$s_func="if(\$selectwerte['projektart'] == FXP_PRJ_MAIN) \$selectwerte['name_projekt']='* '.\$selectwerte['name_projekt'].' ['.no_null(\$selectwerte['vorgangsnummer']).'] {**}'; else { \$selectwerte['name_projekt']='&nbsp;&nbsp;'.no_null(\$selectwerte['vorgangsnummer']).' - '.\$selectwerte['name_projekt'];";
			$s_func .= " if(\$selectwerte['projektart'] == FXP_PRJ_SUB) \$selectwerte['name_projekt'] .= ' {*}'; }";
		}
		// nach Vorgangsname sortieren ohne Prefix
		else
		{
			$s_order=db_orderby('p.name_projekt', 'p.vorgangsnummer');
			$s_func="if(\$selectwerte['projektart'] == FXP_PRJ_MAIN) \$selectwerte['name_projekt']='* '.\$selectwerte['name_projekt'].' ['.no_null(\$selectwerte['vorgangsnummer']).'] {**}'; else { \$selectwerte['name_projekt']='&nbsp;&nbsp;'.no_null(\$selectwerte['vorgangsnummer']).' - '.\$selectwerte['name_projekt']; if(\$selectwerte['projektart'] == FXP_PRJ_SUB) \$selectwerte['name_projekt'] .= ' {*}'; }";
		}

		// Soll-Zeiträume
		if($s_feldname == 'Abhaengigkeit')
			$s_func	.= '$selectwerte["name_projekt"] .= " (".get_entry($selectwerte["soll_beginn"], "datum")." - ".get_entry($selectwerte["soll_ende"], "datum").")";';

		// Nur bestimmte Projektart auswählbar?
		if(isset($vprojektart) && strlen($vprojektart))
			$s_func .= ' if($selectwerte["projektart"] != '.$vprojektart.') $selectwerte["name_projekt"] .= "<d>";';

		// Recht auf Mutterprojekt in Detail/Matrix?
		if($GLOBALS['fxpglobals']['person'] && ($GLOBALS['fxpglobals']['user'] != 1) && !$GLOBALS['fxpglobals']['persdat']['roles'][2] && (($GLOBALS['fxpglobals']['tr'] == 23) || ($GLOBALS['fxpglobals']['tr'] == 189)) && (($GLOBALS['aktion'] == FXP_CHANGE) || ($GLOBALS['aktion'] == FXP_DELETE)) && (strtolower(substr($GLOBALS['fxpglobals']['settings']['pm_aendert_mp'], 0, 1)) == 'j'))
			$s_func .= ' if(($selectwerte["projektart"] == '.FXP_PRJ_MAIN.') && ($selectwerte["projektmanager"] != '.$GLOBALS['fxpglobals']['person'].')) $selectwerte["name_projekt"] .= "<red>";';

		if($use_structsel)
		{
			$csql="SELECT COUNT(p.projekt_id) ".$s_from." ".$s_where;
			$scnt=(int)db_value($csql);
echo('Count 1 (SQL): <b>'.$scnt.'</b><hr />');
		}

		$s_name .= " ".$s_order;
	}
//echo('<b>SQL:</b><br />'.fxHtmlEncode($s_name).'<hr />');
	$s_wert='p.projekt_id';
	$s_feld='name_projekt';

	if(!isset($GLOBALS['selpopptype']))
		$GLOBALS['selpopptype']=array();
	$GLOBALS['selpopptype'][$s_feldname]='---';

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_array=array();
	$arg_array=argArray($s_name,$s_func,$s_feld,$s_wert,$s_feldname,$a_arg_array);
	if(fxIsArray($override))
		$a_array=$override;
	else if($use_structsel)
	{
		$tmp=db_values($s_name,'*');
		foreach($tmp as $t)
		{
			if($t['projektart'] == FXP_PRJ_MAIN)
			{
				$a_array[$t['projekt_id']]='<font class=darkgrey>'.no_null($t['vorgangsnummer']).'</font>&nbsp;&nbsp;<b>'.$t['name_projekt'].'</b>';
			}
		}
	}
	else
	{
		$a_array=select_array($arg_array);
	}
//fxDebug($a_array, $s_name);

	if($use_structsel)
	{
		$acnt=sizeof($a_array);
echo('Count 2 (ARRAY): <b>'.$acnt.'</b><hr />');
	}

	// Projektstruktur entspr. Vorgangsnummer einrücken
	if($disp_structure && fxIsArray($a_array))
	{
		foreach($a_array as $pid => $pn)
		{
			if($pid > 0)
			{
				$imp=strpos($pn,'{**}');
				if($imp === false)
				{
					$npn=trim(str_replace('&nbsp;','',$pn));
					$ndc=0;
					$fsp=strpos($npn,' ');
					if($fsp > 0)
					{
						$ndc=substr_count(substr($npn,0,$fsp), '.');
						if($ndc > 0)
						{
							$npn=str_repeat('&nbsp;', $ndc*3).$npn;
							$a_array[$pid]=$npn;
						}
					}
//echo('<b>$disp_structure</b> ['.fxHtmlEncode($pn).'] &rarr; $ndc='.$ndc.' &rarr; ['.fxHtmlEncode($npn).']<br />');
				}
			}
		}
	}

	$_STA[1]=microtime_double();
	if($use_structsel)
	{
		echo('<font class="blue">Time 1:</font> <font class="s2 lightgrey">'.$_STA[1].'</font> - <font class="s2">'.($_STA[1]-$_STA[0]).' sec</font><br />');
		show_memory_usage(__LINE__, __FILE__, true);
	}

	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);

	$_STA[2]=microtime_double();
	if($use_structsel)
	{
		echo('<font class="blue">Time 2:</font> <font class="s2 lightgrey">'.$_STA[2].'</font> - <font class="s2">'.($_STA[2]-$_STA[0]).' sec</font><br />');
		show_memory_usage(__LINE__, __FILE__, true);
	}

	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):           ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):     ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'ProjektID'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_ProjektID($a_return=0, $a_arg_array=array(), $s_feldname='ProjektID')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return auswahl_Projekt_ID($a_return, $a_arg_array, $s_feldname);
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):                ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):          ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Projekt_IDopen'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Projekt_IDopen($a_return=0, $a_arg_array=array(), $s_feldname='Projekt_IDopen')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return auswahl_Projekt_ID($a_return, $a_arg_array, $s_feldname);
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):                   ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):             ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Projekt_HistoryID'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Projekt_HistoryID($a_return=0, $a_arg_array=array(), $s_feldname='Projekt_HistoryID')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return auswahl_Projekt_ID($a_return, $a_arg_array, $s_feldname);
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):               ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):         ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Trans_Projekt'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Trans_Projekt($a_return=0, $a_arg_array=array(), $s_feldname='Trans_Projekt')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return auswahl_Projekt_ID($a_return, $a_arg_array, $s_feldname);
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):              ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):        ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Projekt_Team'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Projekt_Team($a_return=0, $a_arg_array=array(), $s_feldname='Projekt_Team')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return auswahl_Projekt_ID($a_return, $a_arg_array, $s_feldname);
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):             ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):       ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'fuerVorgang'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_fuerVorgang($a_return=0, $a_arg_array=array(), $s_feldname='fuerVorgang')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return auswahl_Projekt_ID($a_return, $a_arg_array, $s_feldname);
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):               ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):         ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Abhaengigkeit'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Abhaengigkeit($a_return=0, $a_arg_array=array(), $s_feldname='Abhaengigkeit')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return auswahl_Projekt_ID($a_return, $a_arg_array, $s_feldname);
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):               ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):         ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Benutzergr_ID'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Benutzergr_ID($a_return=0, $a_arg_array=array(), $s_feldname='Benutzergr_ID')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];
	if(!$Switch)
		$Switch=0;

	// $a_array direkt als Array übergeben
	$a_array=array();
	if(fxIsArray($arr))
		$a_array=$arr;
	else
	{
		// Select-Argumente vorbereiten
		switch($Switch)
		{
			case 0:
				$a_array=array();

				// Benutzergruppen holen
				$sql="SELECT b.benutzergr_id AS id, w.tabwert AS txt FROM benutzergruppen b, wertetab w WHERE b.gruppenname_id=w.wertetabid AND b.mandanten_id";
				if($null_idx == true)
					$sql .= " IN (0,".$Mandanten_ID.")";
				else
					$sql .= "=".$Mandanten_ID;
				$sql .= " AND w.id_sprache=".$GLOBALS['fxpglobals']['lang']." AND w.mandanten_id IN (0,".$Mandanten_ID.") ".db_orderby('tabwert');
				$arr=db_values($sql, '*');
//fxDebug($arr, $sql);
				if(fxIsArray($arr))
				{
					foreach($arr as $a)
						$a_array[$a['id']]=$a['txt'];
				}
//fxDebug($a_array, '$a_array');
			break;

			case 1:
				return auswahl_Benutzer_ID($a_return, $a_arg_array, 'Benutzergr_ID');
			break;
		}
	}
//fxDebug($a_array,'$a_array');
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):               ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):         ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'defaultaktion'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_defaultaktion($a_return=0, $a_arg_array=array(), $s_feldname='defaultaktion')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

	// Select-Argumente vorbereiten
	$s_name="SELECT tabwert, position FROM wertetab WHERE id_feld=332 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id=0 AND position<5 ORDER BY position";
	$s_func='$selectwerte["tabwert"]=$selectwerte["position"]." - ".$selectwerte["tabwert"];';
	$s_wert='position';
	$s_feld='tabwert';

	// Auswahl-Array erstellen
	$arg_array=argArray($s_name,$s_func,$s_feld,$s_wert,$s_feldname,$a_arg_array);
	$a_array=select_array($arg_array);
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):             ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):       ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Personen_ID'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Personen_ID($a_return=0, $a_arg_array=array(), $s_feldname='Personen_ID')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$turn=false;
	$prem=true;

	if(!isset($GLOBALS['pid_tr']))
		$GLOBALS['pid_tr']=array('b'=>0, 'a'=>0);

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);

	if(isset($override) && ($a_return != -1))
	{
		$turn=true;
		$prem=false;
		$a_array=$override;
		$flt=$GLOBALS[$s_feldname.'_flt'];

		if($flt)
			$GLOBALS['mskfilter'] |= $flt;
	}
	else
	{
		if(!$Mandanten_ID)
			$Mandanten_ID=$GLOBALS['fxpglobals']['client'];
		if(!$Personen_ID)
			$Personen_ID=$GLOBALS['fxpglobals']['person'];
		if(!$modus)
			$modus=0;

		$s_wert='';

		$mint=' ['.get_text(1225, '5').']';		// [Int.]
		$mext=' ['.get_text(1229, '5').']';		// [Ext.]
		$inact=' {'.strtolower(get_text(1391, '20')).'}';	// {inactive}

		// Select-Argumente vorbereiten
//echo('<b>'.$s_feldname.':</b> $modus='.$modus.'<br />');
		switch($modus)
		{
			case 1:
				$s_name="SELECT p.personen_id";
				if($a_return != -1)
					$s_name .= ", p.pname, p.vorname, p.personen_initialen, p.geschlecht, p.personentyp, p.inaktiv, d.maeintrittsdatum, d.maaustrittsdatum";
				$s_name .= " FROM personen p LEFT JOIN madaten d ON (d.personen_id=p.personen_id AND d.mandanten_id=p.mandanten_id)";
				$s_name .= " WHERE p.mandanten_id=".$Mandanten_ID;
				if(fxIsArray($Pt))
				{
					$typ='';
					foreach($Pt as $p)
					{
						if($typ)
							$typ .= ',';
						$typ .= (int)$p;
					}
					$s_name .= " AND p.personentyp IN (".$typ.")";
				}
				// Auf Personenrechte einschränken, ausser in Mandanten-Transaktion
				$pflt='';
				if(($GLOBALS['fxpglobals']['tr'] != 166) && (!isset($ignore_personenfilter) || !$ignore_personenfilter) && (!isset($GLOBALS['ignore_personenfilter']) || !$GLOBALS['ignore_personenfilter']))
				{
					$pflt=filter_person('and', true, false, $s_feldname);
					$s_name .= $pflt;
				}
				if($a_return == -1)
					return $s_name;
				$s_name .= " ".db_orderby('pname', 'vorname');
			break;

			case 2:
//fxDebug($ptpa,'$ptpa', 0);
				$turn=true;
				// Include person?
				$sql_pid="";
				if(isset($pid) && strlen($pid))
				{
					$up=strpos($pid,'_');
					if($up)
					{
						$cpid=(int)substr($pid,0,$up);
						$ppid=(int)substr($pid,$up+1);
					}
					else
					{
						$cpid=$GLOBALS['fxpglobals']['client'];
						$ppid=(int)$pid;
					}
					$sql_pid=" OR (p.personen_id=".$ppid." AND p.mandanten_id=".$cpid.")";
				}

				// No client id before person id?
				$noclient=0;
				if((fxIsArray($ptpa) && isset($ptpa['noclient']) && $ptpa['noclient']) || (isset($GLOBALS['with_client_id']) && !$GLOBALS['with_client_id']))
					$noclient=1;

				// All clients
				$sql_cl="";
				if(fxIsArray($ptpa) && isset($ptpa['all_clients']) && !$ptpa['all_clients'])
					$sql_cl=" AND p.mandanten_id=".(int)$GLOBALS['fxpglobals']['client'];

				// Person types
				$pt='';
				if(fxIsArray($ptpa) && isset($ptpa['persontypes']))
				{
					$pt='';
					if(is_array($ptpa['persontypes']))
					{
						if(sizeof($ptpa['persontypes']))
						{
							foreach($ptpa['persontypes'] as $apt)
							{
								if(strlen($pt))
									$pt .= ",";
								$pt .= (int)$apt;
							}
						}
					}
					else
						$pt=trim($ptpa['persontypes']);
				}
				$sql_pt="";
				if(strlen($pt))
					$sql_pt=" AND p.personentyp IN (".$pt.")";

				// Filter?
				$sql_flt="";
				if(fxIsArray($ptpa) && isset($ptpa['filter']) && strlen($ptpa['filter']))
					$sql_flt=" ".trim($ptpa['filter']);

				$sql  = "SELECT p.personen_id, p.mandanten_id, p.pname, p.vorname, p.personen_initialen, p.elter, p.personentyp, p.geschlecht, p.inaktiv,";
				$sql .= " w.tabwert,";
				$sql .= " d.maeintrittsdatum, d.maaustrittsdatum";
				$sql .= " FROM personen p INNER JOIN wertetab w ON (w.wertetabid=p.personentyp AND w.id_sprache=".$GLOBALS['fxpglobals']['lang']." AND w.mandanten_id=0) LEFT JOIN madaten d ON (d.personen_id=p.personen_id AND d.mandanten_id=p.mandanten_id)";
				$sql .= " WHERE ";
				if(strlen($sql_pid))
					$sql .= "(";
				if(strlen($sql_cl))
					$sql .= substr($sql_cl,5);
				else
					$sql .= "p.mandanten_id>0";
				$sql .= $sql_pt.$sql_flt;
				if(strlen($sql_pid))
					$sql .= ")".$sql_pid;
				$sql .= db_orderby('p.mandanten_id', 'w.tabwert', 'p.pname', 'p.vorname');
				$parr=db_values($sql, '*');
//fxDebug($parr,$sql, 0);

				// Get display informations
				$max_cn=0;
				$ca=array();
				$ta=array();
				$ra=array();
				$pcsv='';
				$rcsv='';
				$onlyclients=true;
				if(fxIsArray($parr))
				{
					$spa=sizeof($parr);
					foreach($parr as $pk => $pa)
					{
						$pcn=(int)$pa['mandanten_id'];
						if(!isset($ca[$pcn]))
							$ca[$pcn]=array();

						$ppt=(int)$pa['personentyp'];
						if(!isset($ca[$pcn][$ppt]))
							$ca[$pcn][$ppt]=array();

						if(!isset($ta[$ppt]))
							$ta[$ppt]=1;
						else
							$ta[$ppt]++;

						if($ppt != FXP_PT_CLIENT)
							$onlyclients=false;

						$pid=(int)$pa['personen_id'];
						$ca[$pcn][$ppt][$pid]=$pk;
						if(strlen($pcsv))
							$pcsv .= ', ';
						$pcsv .= $pid;

						$rid=(int)$pa['elter'];
						if($rid && !isset($ra[$rid]))
						{
							$ra[$rid]=true;
							if(strlen($rcsv))
								$rcsv .= ', ';
							$rcsv .= $rid;
						}

						$max_cn=max($max_cn,$pcn);
					}
				}
//fxDebug($ca,'$ca: $max_cn='.$max_cn.', $onlyclients='.$onlyclients, 0);
//fxDebug($ta,'$ta', 0);

				// Parent?
				$pra=array();
				if(strlen($rcsv))
				{
					$rsql  = "SELECT mandanten_id, personen_id, pname, vorname, inaktiv";
					$rsql .= " FROM personen";
					$rsql .= " WHERE personen_id IN (".$rcsv.") AND personentyp IN (".FXP_PT_CUSTOMER_CP.",".FXP_PT_PARTNER_CP.",".FXP_PT_CONTACT_CP.")";
					$rarr=db_values($rsql, '*');
//fxDebug($rarr,$rsql, 0);
					if(fxIsArray($rarr))
					{
						foreach($rarr as $ra)
						{
							$rcn=(int)$ra['mandanten_id'];
							if(!isset($pra[$rcn]))
								$pra[$rcn]=array();
							$rpi=(int)$ra['personen_id'];
							$pra[$rcn][$rpi]=$ra['pname'];
							if(strlen($ra['vorname']))
								$pra[$rcn][$rpi] .= ', '.$ra['vorname'];
							if($ra['inaktiv'])
								$pra[$rcn][$rpi] .= $inact;
							$pra[$rcn][$rpi] .= ' &rarr; ';
						}
					}
				}
//fxDebug($pra,'$pra', 0);

				// With user?
				$pua=array();
				if(fxIsArray($ptpa) && isset($ptpa['with_user']) && $ptpa['with_user'] && strlen($pcsv))
				{
					$usql  = "SELECT mandanten_id, personen_id, benutzer_id, name_benutzer, b_admin, status_benutzer, gueltig_von, gueltig_bis";
					$usql .= " FROM benutzer WHERE personen_id IN (".$pcsv.")";
					$uarr=db_values($usql, '*');
//fxDebug($uarr,$usql, 0);
					if(fxIsArray($uarr))
					{
						foreach($uarr as $ua)
						{
							$ucn=(int)$ua['mandanten_id'];
							if(!isset($pua[$ucn]))
								$pua[$ucn]=array();
							$upi=(int)$ua['personen_id'];
							if(!isset($pua[$ucn][$upi]))
							{
								$pua[$ucn][$upi]=' - ';
								if($GLOBALS['fxpglobals']['tr'] == 30)
									$pua[$ucn][$upi] .= '<green></green>';
							}
							else
								$pua[$ucn][$upi] .= ', ';
							$pua[$ucn][$upi] .= '['.$ua['name_benutzer'].']';
							if($ua['b_admin'])
								$pua[$ucn][$upi] .= '*';
						}
					}
				}
//fxDebug($pua,'$pua', 0);

				// Create select
				$mps=getSettingValue('max_pic_sel');
				$spa=0;
				$nid=-1;
				$a_array=array();
				if(sizeof($ca))
				{
					// ...Cylce through each client
					$cc=0;
					foreach($ca as $cid => $csa)
					{
//						if(!$onlyclients && ((sizeof($ca) > 1) || (sizeof($csa) > 1)))
						if(!$onlyclients && (sizeof($ca) > 1))
						{
							$sh=meldung(FXP_PT_CLIENT);
							if($cc)
							{
								$a_array[$nid]='<d>';
								$nid--;
								$a_array[$nid]='<d>';
								$nid--;
							}
							$a_array[$nid]='<b>'.$sh.' '.$cid.'</b><bb><bg#e0e0e0><d>';
							$nid--;
						}

						$spt='';
						if(!$onlyclients && (sizeof($ca) > 1))
							$spt=str_repeat('&nbsp;',3);

						// ...Cylce through each person type
						$pc=0;
						foreach($csa as $ppt => $ppa)
						{
							if(($ppt != FXP_PT_CLIENT) && (sizeof($ta) > 1))
							{
								$sh=meldung($ppt);
								if($pc)
								{
									$a_array[$nid]='<d>';
									$nid--;
								}
								$a_array[$nid]=$spt.'<b>'.$sh.'</b><bb><bg#f0f0f0><d>';
								$nid--;
							}

							$spp=$spt;
							if((sizeof($csa) > 1) && (sizeof($ta) > 1))
								$spp .= str_repeat('&nbsp;',3);

							// ...Cylce through each person
							foreach($ppa as $pid => $pk)
							{
								// Select Id
								if($noclient)
									$id=$pid;
								else
									$id=$cid.'_'.$pid;

								// Select value
								$st='';
								// ...Picture
								if(!$mps || ($spa < $mps))
								{
									$spa++;
									$pmxh=256;
									$st .= getPersonSelectIcon($pid, $pmxh, $parr[$pk]['geschlecht'], $cid, $ppt);
								}
								// ...Client no.
								if(($ppt == FXP_PT_CLIENT) || (!$onlyclients && (sizeof($ca) > 1)))
									$st .= substr(str_repeat('0',strlen($max_cn)).$cid, -strlen($max_cn)).': ';
								// ...Superior/Parent info
								$rid=$parr[$pk]['elter'];
								if(sizeof($pra) && is_array($pra[$cid]) && isset($pra[$cid][$rid]))
									$st .= $pra[$cid][$rid];
								// ...Person name
								$st .= $parr[$pk]['pname'];
								if(strlen($parr[$pk]['vorname']))
									$st .= ', '.$parr[$pk]['vorname'];
								// ...Initials
								if(strlen($parr[$pk]['personen_initialen']))
									$st .= ' "'.$parr[$pk]['personen_initialen'].'"';
								// ...User
								if(sizeof($pua) && is_array($pua[$cid]) && isset($pua[$cid][$pid]))
									$st .= $pua[$cid][$pid];
								// ...Entry/Leave date
								if(strlen($parr[$pk]['maeintrittsdatum']))
								{
									$ed=substr($parr[$pk]['maeintrittsdatum'],0,8);
									$ld='';
									$st .= ' ('.get_entry($ed,'date');
									if(strlen($parr[$pk]['maaustrittsdatum']))
									{
										$ld=substr($parr[$pk]['maaustrittsdatum'],0,8);
										$st .= '-'.get_entry($ld,'date');
									}
									$st .= ')';
									if(($ed > $GLOBALS['date']) || (strlen($ld) && ($ld < $GLOBALS['date'])))
										$st .= '<red></red>';
								}
								// ...Inactive
								if($parr[$pk]['inaktiv'])
									$st .= '<red>'.$inact.'</red>';

								$a_array[$id]=$spp.$st;
							}
							$pc++;
						}
						$cc++;
					}
				}
//fxDebug($a_array,'$a_array', 0);
			break;

			default:
				$s_name="SELECT p.personen_id";
				if($a_return != -1)
					$s_name .= ", p.pname, p.vorname, p.personen_initialen, p.geschlecht, p.personentyp, p.inaktiv, d.maeintrittsdatum, d.maaustrittsdatum";

				if(!isset($ptyp))
					$ptyp=FXP_PT_EMPLOYEE.','.FXP_PT_CONTRACTOR;

				$s_name	.= " FROM personen p LEFT JOIN madaten d ON (d.personen_id=p.personen_id AND d.mandanten_id=p.mandanten_id) WHERE ";
				if($ptyp != 'ALL')
					$s_name .= "p.personentyp IN (".$ptyp.") AND ";
				$s_name .= "p.mandanten_id=".$Mandanten_ID." AND (p.personen_id>0";
				if((!isset($ignore_personenfilter) || !$ignore_personenfilter) && (!isset($GLOBALS['ignore_personenfilter']) || !$GLOBALS['ignore_personenfilter']))
					$s_name .= filter_person('and', true, false, $s_feldname);
				else if($self_only)
					$s_name	.= " AND p.personen_id=".$GLOBALS['fxpglobals']['person'];
				else
				{
					// Recht auf alle Personen
					$all=hasRightsForAllPersons($ignore_personenrechte);
//fxDebug($all,'$all: $ignore_personenrechte='.$ignore_personenrechte);
		 			if(!$all)
		 			{
						$pers_rights_csv=get_persons();
						if(strlen($pers_rights_csv))
							$s_name	.= " AND p.personen_id IN (".$pers_rights_csv.")";
					}
				}
				$s_name .= ")";

				// In Matrix also persons who are already assigned to tasks
				if($mrp)
				{
					if(isset($pn) && strlen($pn))
					{
						$s_name .= " OR ";
						$s_name .= "(";
						$s_name .= "  p.mandanten_id=".$Mandanten_ID." AND p.personen_id IN";
						$s_name .= "  (SELECT personen_id FROM maplanung WHERE (sachmittel_id IS NULL or sachmittel_id=0) AND mandanten_id=".$Mandanten_ID;
						$s_name .= "    AND projekt_id IN (SELECT projekt_id FROM projekte WHERE vorgangsnummer LIKE '".$pn."%' AND mandanten_id=".$Mandanten_ID.")";
						$s_name .= "  )";
						$s_name .= ")";
					}
				}

				if($a_return == -1)
					return $s_name;
				$s_name .= " ".db_orderby('p.pname', 'p.vorname');

				$s_wert='p.personen_id';
			break;
		}
	}

	$s_feld='pname';
	if(!$s_wert)
		$s_wert='personen_id';

	// Auswahl-Array erstellen
	$arg_array=argArray($s_name,$s_func,$s_feld,$s_wert,$s_feldname,$a_arg_array);
	if(!$turn && (!isset($a_array) || !is_array($a_array) || !sizeof($a_array)))
	{
		$a_array=array();
		$Pers_arr=db_values($s_name, '*');
//fxDebug($Pers_arr, $s_name);
		if(fxIsArray($Pers_arr))
		{
			$mps=getSettingValue('max_pic_sel');
			foreach($Pers_arr as $p)
			{
				$p_id=(int)$p['personen_id'];

				$with_icons=false;
				if(!$mps || (sizeof($a_array) < $mps))
					$with_icons=true;
				$a_array[$p_id]=persArray2Text($p,$with_icons);
			}
		}
//		$a_array=select_array($arg_array);
		$GLOBALS['pid_tr']['b']=sizeof($a_array);
	}
	// In Transaktion 81: ZE (Tag), 83: ZE (Woche/Monat), 34: ZE (Summe), 102: Materialerfassung oder 150: Reisekostenerfassung nur Personen anzeigen, welche Recht auf diese Transaktion haben
	if($prem && (($GLOBALS['fxpglobals']['tr'] == 81) || ($GLOBALS['fxpglobals']['tr'] == 83) || ($GLOBALS['fxpglobals']['tr'] == 34) || ($GLOBALS['fxpglobals']['tr'] == 102) || ($GLOBALS['fxpglobals']['tr'] == 150)) && is_array($a_array) && sizeof($a_array))
	{
		$bid=array();
		$sql="SELECT personen_id, benutzer_id FROM benutzer WHERE personen_id IN (".array_to_csv($a_array).") AND mandanten_id=".$Mandanten_ID;
		$tmp=db_values($sql, '*');
		if(is_array($tmp) && sizeof($tmp))
		{
			foreach($tmp as $t)
				$bid[(int)$t['personen_id']][]=(int)$t['benutzer_id'];
			foreach($a_array as $pid => $name)
			{
				$tr_right=0;
				if($pid && isset($bid[$pid]) && $bid[$pid])
				{
					foreach($bid[$pid] as $id)
					{
						if($id)
							$tr_right |= tr_rechte($GLOBALS['fxpglobals']['tr'], $id);
					}
				}
//echo('-&gt; FINAL: '.$pid.'='.$tr150.'<br />');
				if(!$tr_right)
					unset($a_array[$pid]);
			}
		}
		$GLOBALS['pid_tr']['a']=sizeof($a_array);
	}
//fxDebug($a_array, $s_name);
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param various $p          - Mandatory parameter:                  ???PARAMETER???
 * @param boolean $with_icons - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function persArray2Text($p, $with_icons=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!isset($GLOBALS['_pt_mint']))
		$GLOBALS['_pt_mint']=' ['.get_text(1225, '5').']';		// [Int.]
	if(!isset($GLOBALS['_pt_mext']))
		$GLOBALS['_pt_mext']=' ['.get_text(1229, '5').']';		// [Ext.]
	if(!isset($GLOBALS['_pt_inakt']))
		$GLOBALS['_pt_inakt']=' {'.get_text(1391, '20').'}';	// {inaktiv}

//fxDebug($p,'$p', 0);
	$p_mid=(int)$p['mandanten_id'];
	$p_elter=(int)$p['elter'];
	$p_mw=(int)$p['geschlecht'];
	$p_typ=(int)$p['personentyp'];
	$p_inaktiv=(int)$p['inaktiv'];
	$p_name=trim($p['pname']);
	$p_vorname=trim($p['vorname']);
	$p_init=trim($p['personen_initialen']);

	if(isset($p['maeintrittsdatum']) && ((substr($p['maeintrittsdatum'],0,8) > substr($GLOBALS['datetime'],0,8)) || (strlen($p['maaustrittsdatum']) && (substr($p['maaustrittsdatum'],0,8) < substr($GLOBALS['datetime'],0,8)))))
		$rtext='<red>';
	else
		$rtext='';

	if($with_icons && ($GLOBALS['fxpglobals']['tr'] != 189))
	{
		$pmxh=256;
		$rtext .= getPersonSelectIcon($p['personen_id'], $pmxh, $p_mw, $p_mid, $p_typ);
	}

	$rtext .= $p_name;
	if(strlen($p_vorname))
		$rtext .= ', '.$p_vorname;

	if(strlen($p_init))
		$rtext .= ' - '.$p_init;

	if($p_typ == FXP_PT_EMPLOYEE)
		$rtext .= $GLOBALS['_pt_mint'];
	else if($p_typ == FXP_PT_CONTRACTOR)
		$rtext .= $GLOBALS['_pt_mext'];

	if(isset($p['maeintrittsdatum']) && strlen($p['maeintrittsdatum']))
	{
		$rtext .= ' ('.get_entry($p['maeintrittsdatum'],'datum');
		if(strlen($p['maaustrittsdatum']))
			$rtext .= '-'.get_entry($p['maaustrittsdatum'],'datum');
		$rtext .= ')';
	}

	if($p_inaktiv)
		$rtext .= $GLOBALS['_pt_inakt'];

	return $rtext;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):                   ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):             ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Personen_ID_alias'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Personen_ID_alias($a_return=0, $a_arg_array=array(), $s_feldname='Personen_ID_alias')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return auswahl_Personen_ID($a_return, $a_arg_array, $s_feldname);
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):              ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):        ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Person_alias'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Person_alias($a_return=0, $a_arg_array=array(), $s_feldname='Person_alias')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return auswahl_Personen_ID($a_return, $a_arg_array, $s_feldname);
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):                    ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):              ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Mitarbeitertermine'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Mitarbeitertermine($a_return=0, $a_arg_array=array(), $s_feldname='Mitarbeitertermine')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return auswahl_Personen_ID($a_return, $a_arg_array, $s_feldname);
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):             ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):       ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Benutzer_ID'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Benutzer_ID($a_return=0, $a_arg_array=array(), $s_feldname='Benutzer_ID')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

	// Select-Argumente vorbereiten
	$s_name  = "SELECT b.benutzer_id, b.name_benutzer, b.status_benutzer, b.b_admin, b.gueltig_von, b.gueltig_bis, p.personen_id, p.pname, p.vorname, p.geschlecht, p.personentyp, p.personen_initialen, p.inaktiv, w.tabwert";
	$s_name .= " FROM benutzer b, personen p, wertetab w";
	$s_name .= " WHERE p.personentyp=w.wertetabid AND p.mandanten_id=".$Mandanten_ID." AND w.id_sprache=".$GLOBALS['fxpglobals']['lang']." AND w.mandanten_id IN (0,".$Mandanten_ID.")";
	$s_name .= " AND b.personen_id=p.personen_id AND b.mandanten_id=".$Mandanten_ID;
	if($Benutzer_ID)
		$s_name .= " AND b.benutzer_id=".$Benutzer_ID;
	else
	{
		$filter=filter_person('', false, false, $s_feldname, false, false);
		if(strlen($filter))
		{
			$fnl=strtolower($s_feldname);
			$elid=0;
			if(fxIsArray($GLOBALS['maskenfeldwerte']) && isset($GLOBALS['maskenfeldwerte'][$fnl]))
				$elid=(int)$GLOBALS['maskenfeldwerte'][$fnl];
			$s_name .= ' AND ('.$filter;
			if($elid && ($GLOBALS['fxpglobals']['action'] != FXP_CREATE))
				$s_name .= ' OR p.personen_id='.$elid;
			$s_name .= ')';
		}
	}
	if($GLOBALS['fxpglobals']['action'] == FXP_DELETE)
		$s_name .= " AND b.benutzer_id>1";
	$s_name .= ' '.db_orderby('w.tabwert', 'p.pname', 'p.vorname', 'b.name_benutzer');
	$arr=db_values($s_name,'*');
//fxDebug($arr,$s_name, 0);

	$a_array=array();
	if(fxIsArray($arr))
	{
		$inactive=' {'.strtolower(get_text(1391, '20')).'}';	// {inactive}
		$locked=' {'.strtolower(get_text(441, '20')).'}';		// {locked}
		$mps=getSettingValue('max_pic_sel');
		$spa=0;
		$sh='';
		$si=-1;
		foreach($arr as $a)
		{
			if($a['tabwert'] != $sh)
			{
				if(sizeof($a_array))
				{
					$a_array[$si]='<d>';
					$si--;
				}
				$sh=$a['tabwert'];
				$a_array[$si]='<b>'.$sh.'</b><bb><bg#f0f0f0><d>';
				$si--;
			}

			// Select value
			$st='';
			// ...Picture
			if(!$mps || ($spa < $mps))
			{
				$spa++;
				$pmxh=256;
				$st .= getPersonSelectIcon($a['personen_id'], $pmxh, $a['geschlecht'], $Mandanten_ID, $a['personentyp']);
			}
			// ...Person name
			$st .= $a['pname'];
			if(strlen($a['vorname']))
				$st .= ', '.$a['vorname'];
			// ...Initials
			if(strlen($a['personen_initialen']))
				$st .= ' "'.$a['personen_initialen'].'"';
			// ...User
			$st .= ' - ['.$a['name_benutzer'].']';
			if($a['b_admin'])
				$st .= '*';
			// ...Inactive
			if($a['inaktiv'] || ($a['status_benutzer'] == FXP_USER_INACTIVE))
				$st .= '<red>'.$inactive.'</red>';
			// ...Locked
			else if($a['status_benutzer'] == FXP_USER_LOCKED)
				$st .= $locked;
			$a_array[$a['benutzer_id']]=$st;
		}
	}

	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):        ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):  ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Abt_ID'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Abt_ID($a_return=0, $a_arg_array=array(), $s_feldname='Abt_ID')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	if(isset($GLOBALS[$s_feldname.'_arg']))
	{
		$arg=$GLOBALS[$s_feldname.'_arg'];		// Argumente-Array in Hauptprogramm: $FELDNAME_arg=array(..., ...);
		if(fxIsArray($arg))
			extract($arg);
	}
	if(!isset($Mandanten_ID) || !$Mandanten_ID)		// Mandanten_ID, kann im Argumente-Array mitgegeben werden
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

	// Select-Argumente vorbereiten
	$s_name="SELECT abt_id, zu_abteilung, abt_name, abt_ebene FROM abteilung WHERE mandanten_id=".$Mandanten_ID." ".db_orderby('abt_ebene','abt_name');
	$s_feld='abt_name';
	$s_wert='abt_id';

	// Auswahl-Array erstellen
	$arg_array=array('s_name'=>$s_name, 's_func'=>"", 's_feld'=>$s_feld, 's_wert'=>$s_wert, 'a_name'=>$s_feldname, 'a_select'=>$a_arg_array['a_select'], 'a_change'=>$a_arg_array['a_change']);
	$a_array=array(0 => '*'.db_value("SELECT pname FROM personen WHERE personentyp=274 AND mandanten_id=".$Mandanten_ID));
	$tmp=getDepStructure('',true,$Mandanten_ID);
//fxDebug($tmp,'$tmp');
	if(fxIsArray($tmp))
	{
		foreach($tmp as $tc => $ta)
		{
			$nam=$ta['abt_ebene'].': '.$ta['abt_name'];
			if($ta['persons'] > 0)
				$nam .= ' ('.$ta['persons'].'x)';

			$a_array[$ta['abt_id']]=trim(str_repeat('...', $ta['level']).' '.$nam);
		}
	}
//fxDebug($a_array,'$a_array');

	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):              ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):        ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Abt_ID_alias'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Abt_ID_alias($a_return=0, $a_arg_array=array(), $s_feldname='Abt_ID_alias')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return auswahl_Abt_ID($a_return, $a_arg_array, $s_feldname);
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):              ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):        ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'hat_angelegt'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_hat_angelegt($a_return=0, $a_arg_array=array(), $s_feldname='hat_angelegt')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);

	// Select-Argumente vorbereiten
	$s_name='SELECT benutzer_id, name_benutzer FROM benutzer WHERE mandanten_id=10 ORDER BY name_benutzer';
	$s_func='';
	$s_wert='benutzer_id';
	$s_feld='name_benutzer';

	// Auswahl-Array erstellen
	$arg_array=argArray($s_name,$s_func,$s_feld,$s_wert,$s_feldname,$a_arg_array);
	$a_array=select_array($arg_array);
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):            ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):      ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Zustaendig'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Zustaendig($a_return=0, $a_arg_array=array(), $s_feldname='Zustaendig')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return auswahl_hat_angelegt($a_return, $a_arg_array, $s_feldname);
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):          ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):    ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Trans_ID'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Trans_ID($a_return=0, $a_arg_array=array(), $s_feldname='Trans_ID')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];
	// Select-Argumente vorbereiten
	$s_name="SELECT transakt_id, transaktionsname FROM trans ".db_orderby("transaktionsname");	// SELECT-SQL, kann auch bereits fertiges Array sein
	$s_func='';	// Funktion zum Ändern der Selectwerte
	$s_wert='transakt_id';	// SELECT-WERT		Array-Aufbau:
	$s_feld='transaktionsname';	// SELECT-FELD		$a_array[s_wert]="s_feld";

	// Auswahl-Array erstellen
	$arg_array=argArray($s_name,$s_func,$s_feld,$s_wert,$s_feldname,$a_arg_array);
	$a_array=select_array($arg_array);
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):                ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):          ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Trans_Person_1'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Trans_Person_1($a_return=0, $a_arg_array=array(), $s_feldname='Trans_Person_1')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];
	if(!$modus)
		$modus=0;

	// Select-Argumente vorbereiten
	$s_name="SELECT personen_id, pname, vorname, tabwert FROM personen, wertetab WHERE personentyp=wertetabid AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND personen.mandanten_id=".$Mandanten_ID;
	if(is_array($Pt))
	{
		$s_name .= " AND (";
		for($i=0; $Pt[$i]; $i++)
		{
			if($i)
				$s_name .= " OR ";
				$s_name .= "personentyp=".$Pt[$i];
		}
		$s_name .= ")";
	}
	$s_name .= " ".db_orderby('pname', 'vorname');
	$s_func='if($selectwerte["vorname"] != "") $selectwerte["pname"] .= ", ".$selectwerte["vorname"];$selectwerte["pname"] .= " (".$selectwerte["tabwert"].")";';

	$s_feld='pname';
	$s_wert='personen_id';

	// Auswahl-Array erstellen
	$arg_array=argArray($s_name,$s_func,$s_feld,$s_wert,$s_feldname,$a_arg_array);
	$a_array=select_array($arg_array);
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):                ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):          ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Trans_Person_2'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Trans_Person_2($a_return=0, $a_arg_array=array(), $s_feldname='Trans_Person_2')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];
	if (!$modus)
		$modus=0;

	// Select-Argumente vorbereiten

	$s_name="SELECT Personen_ID, Pname, Vorname, Tabwert from Personen, Wertetab where PersonenTyp=Wertetabid and ID_Sprache=".$GLOBALS['fxpglobals']['lang']." and Personen.Mandanten_ID=$Mandanten_ID";
	if($Pt)
	{
		$s_name .=" and (";
		for($i=0; $Pt[$i]; $i++)
		{
			if($i != 0)
				$s_name .=" or ";
			$s_name .="PersonenTyp=".$Pt[$i];
		}
		$s_name .=")";
	}
	$s_name .= " ".db_orderby("Pname", "Vorname");
	$s_func='if($selectwerte["vorname"] != "") $selectwerte["pname"] .= ", ".$selectwerte["vorname"];$selectwerte["pname"] .= " (".$selectwerte["tabwert"].")";';

	$s_feld="pname";
	$s_wert="personen_id";

	// Auswahl-Array erstellen
	$arg_array=argArray($s_name,$s_func,$s_feld,$s_wert,$s_feldname,$a_arg_array);
	$a_array=select_array($arg_array);
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):       ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()): ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Elter'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Elter($a_return=0, $a_arg_array=array(), $s_feldname='Elter')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	global $PS_additional_lines;

	$feld_elter_array=$GLOBALS['fxptdata']['backup']['elter_array'];
	// Zwischenspeichern der Projektkriterien NUR in "Projekte definieren"
	if($feld_elter_array && ($GLOBALS['fxpglobals']['tr'] == 23))
		$auswahlkriterien=$feld_elter_array;
	else
		$auswahlkriterien=$GLOBALS['fxpglobals']['filter'];

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	$a_array=false;
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];
	if(!isset($modus))
	{
		$modus=0;
		if(isset($Modus))
			$modus=(int)$Modus;
	}
	else
		$modus=(int)$modus;
//echo('<b>modus:</b> '.$modus.'<br />');
	switch($modus)
	{
		case 0:
			// $s_name direkt als Array übergeben
			if($comp_array)
			{
				$s_name=$comp_array;
				$s_wert='p.projekt_id';
				$s_feld='name_projekt';
			}
			else
			{
				$s_name ="SELECT name_projekt, p.projekt_id, vorgangsnummer, projektart FROM projekte p, zeitdaten z";
				$s_name .= " WHERE p.mandanten_id=".$Mandanten_ID." AND p.mandanten_id=z.mandanten_id AND p.zeit_id=z.zeit_id";
				if(isset($alle_vorgangsnummern) && strlen($alle_vorgangsnummern))
					$s_name .= " AND p.vorgangsnummer LIKE '".$alle_vorgangsnummern."%'";
				else
					$s_name .= " AND p.projektart<=".FXP_PRJ_SUB;
				if(isset($projektart) && strlen($projektart))
					$s_name .= " AND p.projektart IN (".$projektart.")";

				// Ausgecheckte Projekte auflisten?
				if(prjValidPF())
				{
					$cops=prjGetCheckedOut();
//echo('$cops='.$cops.'<br />');
					if(strlen($cops))
						$s_name .= " AND p.mutterprojekt_id NOT IN (".$cops.")";
				}

				// !!! Dummy-Eintrag um Setzen der Klammern im variablen SQL-Call zu erleichtern !!!
				$s_name .= " AND (p.projekt_id=p.projekt_id".filter_projekt('and', true, false, $s_feldname);

				// Im Elterfeld darf das gewählte Projekt, sowie die Untervorgänge nicht erscheinen,
				// da sonst Zirkelbezug. In der projekte_ini.inc wird der Wert des Projekt_IDopen Feldes
				// in ['Backup']['Projekt_IDopen'] gespeichert und dieser Funktion übergeben
				if($auswahlkriterien['Backup']['Projekt_IDopen'])
					$s_default=" AND p.projekt_id NOT IN (".inferior_vn($auswahlkriterien['Backup']['Projekt_IDopen']).")";
				if($auswahlkriterien['Backup']['Elter'])
					$s_default .= " OR p.projekt_id=".$auswahlkriterien['Backup']['Elter'];

				// Wie oben, nur besser! Für "Rest" (irgendwann Projekte auf diesen Teil umstellen!)
				if($custom['Elter'])
				{
					// Im Elterfeld darf das gewählte Projekt, sowie die Untervorgünge nicht erscheinen,
					if($custom['Elter']['exclude'])
						$s_default=" AND p.projekt_id NOT IN (".inferior_vn($custom['Elter']['exclude']).")";
					// Aktuellen Elter immer in Liste anzeigen
					if($custom['Elter']['include'])
						$s_default .= " OR p.projekt_id=".$custom['Elter']['include'];
				}

				// kommaseparierter String der alle Projekt-IDs enthält, auf die der User Zugriff hat
				// Wenn User nicht Geschäftsführer und kein Sysadmin => Rechteprüfung (außer Qualitätsmanagment-Programm [Romina])
				if($GLOBALS['fxpglobals']['person'] && ($GLOBALS['fxpglobals']['user'] > 1) && !$GLOBALS['fxpglobals']['persdat']['roles'][2])
					$s_name .= " AND p.projekt_id IN (".prights_get(0).")";
				if($s_default)
					$s_name .= $s_default;

				// nach Projektnummern sortieren
				if(strtolower($GLOBALS['fxpglobals']['settings']['zeit_projektnr']) == 'ja')
				{
					$s_name .= ") ".db_orderby('p.vorgangsnummer');
					$s_func='$selectwerte["name_projekt"]=no_null($selectwerte["vorgangsnummer"])." - ".$selectwerte["name_projekt"];';
				}
				else
				{
					$s_name .= ") ".db_orderby('p.name_projekt', 'p.vorgangsnummer');
					$s_func='$selectwerte["name_projekt"] .= " [".no_null($selectwerte["vorgangsnummer"])."]";';
				}
				$s_func .= ' if($selectwerte["projektart"] == FXP_PRJ_MAIN) $selectwerte["name_projekt"] .= " {**}"; else if($selectwerte["projektart"] == FXP_PRJ_SUB) $selectwerte["name_projekt"] .= " {*}";';

				// Nur bestimmte Projektart auswählbar?
				if(isset($vprojektart) && strlen($vprojektart))
					$s_func .= ' if($selectwerte["projektart"] != '.$vprojektart.') $selectwerte["name_projekt"] .= "<d>";';
				$s_func .= ' $lvl=floor(strlen($selectwerte["vorgangsnummer"])/5); if($lvl) $selectwerte["name_projekt"] = str_repeat("&nbsp;", $lvl*3).$selectwerte["name_projekt"];';

//echo('<b>SQL:</b><br />'.fxHtmlEncode($s_name).'<hr />');
				$s_wert='p.projekt_id';
				$s_feld='name_projekt';
				$s_name=db_values($s_name, '*');
				$num=0;
				if(fxIsArray($s_name))
					$num=sizeof($s_name);
			}
//fxDebug($s_name,'$s_name');

			if(!isset($GLOBALS['selpopptype']))
				$GLOBALS['selpopptype']=array();
			$GLOBALS['selpopptype'][$s_feldname]='---';
		break;

		case 1:
			$a_array=array();
			if(is_array($ptyp_arr['ptyp_o']))
			{
				$strg='';
				foreach($ptyp_arr['ptyp_o'] as $pt)
				{
					if(strlen($strg))
						$strg .= ",";
					$strg .= $pt;
				}
				$strg="p.personentyp IN (".$strg.")";
			}
			else
			{
				$strg="p.personentyp IN (275,840)";
				$ptyp_arr['mand']=1;
			}
			$sql="SELECT p.personen_id, p.pname, p.vorname, p.inaktiv, p.mandanten_id FROM personen p WHERE ".$strg;
 			if((isset($ignore_personenrechte) && $ignore_personenrechte) || (isset($GLOBALS['ignore_personenrechte']) && $GLOBALS['ignore_personenrechte']))
 				$sql .= " AND p.personen_id IN (".get_persons().")";
			if($ptyp_arr['mand'] == 1)
				$sql .= " AND p.mandanten_id=".$Mandanten_ID;
			else
				$sql .= " AND p.mandanten_id>0";
			$filter=filter_person('', false, false, $s_feldname, false, false);
			if(strlen($filter))
			{
				$fnl=strtolower($s_feldname);
				$elid=0;
				if(fxIsArray($GLOBALS['maskenfeldwerte']) && isset($GLOBALS['maskenfeldwerte'][$fnl]))
					$elid=(int)$GLOBALS['maskenfeldwerte'][$fnl];
				$sql .= ' AND ('.$filter;
				if($elid && ($GLOBALS['fxpglobals']['action'] != FXP_CREATE))
					$sql .= ' OR p.personen_id='.$elid;
				$sql .= ')';
			}
			$sql .= " ".db_orderby('mandanten_id', 'pname', 'vorname');
			$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql);
			if(is_array($tmp))
			{
				$inakt=' {'.get_text(1391, '20').'}';	// {inaktiv}
				$nrep='';
				if(!$ptyp_arr['mand'])
				{
					$mid=(int)db_value("SELECT MAX(mandanten_id) FROM personen WHERE personentyp=274");
					$len=strlen($mid);
					$nrep=str_repeat('0', $len);
				}
				foreach($tmp as $i => $t)
				{
					$a_array[$t['personen_id']]='';
					if($nrep)
						$a_array[$t['personen_id']] .= substr($nrep.$t['mandanten_id'], -$len).' - ';
					$a_array[$t['personen_id']] .= $t['pname'];
					if(strlen(trim($t['vorname'])))
						$a_array[$t['personen_id']] .= ', '.trim($t['vorname']);
					if($t['inaktiv'])
						$a_array[$t['personen_id']] .= $inakt;
				}
			}
		break;

		case 2:
			$a_array=auswahl_Personen_ID(1, $a_arg_array, $s_feldname);
		break;
	}

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$arg_array=argArray($s_name,$s_func,$s_feld,$s_wert,$s_feldname,$a_arg_array);
	if($a_array === false)
		$a_array=select_array($arg_array);
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):            ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):      ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Projektart'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Projektart($a_return=0, $a_arg_array=array(), $s_feldname='Projektart')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

	if(fxIsArray($arr))
		$a_array=$arr;
	else
	{
		$s_name  = "SELECT k.kategorie_id, w.wertetabid, w.tabwert FROM kategorien k, wertetab w";
		$s_name .= " WHERE k.masterkategorie=57 AND k.mandanten_id=0 AND w.position<=9004 AND w.id_sprache=".$GLOBALS['fxpglobals']['lang']." AND w.mandanten_id=0";
		$s_name .= " AND w.wertetabid=k.name_kategorie AND k.kategorie_id<>k.masterkategorie ".db_orderby("k.kategorie_id");

		//echo('<B><U>SQL:</U></B><PRE>'.fxHtmlEncode($s_name).'</PRE>');
		$s_wert='kategorie_id';
		$s_feld='tabwert';

		// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
		$arg_array=argArray($s_name,$s_func,$s_feld,$s_wert,$s_feldname,$a_arg_array);

		$a_array=select_array($arg_array);
	}
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):          ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):    ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'MaArt_ID'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_maart_id($a_return=0, $a_arg_array=array(), $s_feldname='MaArt_ID')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

	$gruppe=db_values("SELECT maart_id, maart_name FROM ma_art WHERE mandanten_id IN (0,".$Mandanten_ID.") AND id_sprache=".$GLOBALS['fxpglobals']['lang'].db_orderby('maart_name'), '*');
	$faehigkeiten=db_values("SELECT maart_id, mafaehigkeit_id, mafaehigkeit_name FROM ma_faehigkeit WHERE mandanten_id IN (0,".$Mandanten_ID.") AND id_sprache=".$GLOBALS['fxpglobals']['lang'].db_orderby('mafaehigkeit_name'), '*');

	$i=0;
	$f=0;
	if(fxIsArray($gruppe))
	{
		foreach($gruppe as $a_gruppe)
		{
			if($i)
			{
				$s_name[$i]['maart_id']=-10*($i+1);
				$s_name[$i]['maart_name']='<d>';
				$i++;
			}

			$s_name[$i]['maart_id']=$a_gruppe['maart_id'];
			$s_name[$i]['maart_name']='<b>'.$a_gruppe['maart_name'].'</b>';
			$i++;

			if(fxIsArray($faehigkeiten))
			{
				foreach($faehigkeiten as $a_faehigkeiten)
				{
					if($a_faehigkeiten['maart_id'] == $a_gruppe['maart_id'])
					{
						$s_name[$i]['maart_id']=$a_faehigkeiten['mafaehigkeit_id'].'f';
						$s_name[$i]['maart_name']='&nbsp;&nbsp;&nbsp;'.$a_faehigkeiten['mafaehigkeit_name'];
						$i++;
						$f++;
					}
				}
			}
		}
	}
//fxDebug($s_name,'$s_name', 0);

	// Select-Argumente vorbereiten
	$s_wert='maart_id';
	$s_feld='maart_name';	// $a_array[s_wert]="s_feld";

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$arg_array=argArray($s_name,$s_func,$s_feld,$s_wert,$s_feldname,$a_arg_array);
	$a_array=select_array($arg_array);
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):                 ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):           ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'MaFaehigkeit_ID'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_mafaehigkeit_id($a_return=0, $a_arg_array=array(), $s_feldname='MaFaehigkeit_ID')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return auswahl_maart_id($a_return, $a_arg_array, $s_feldname);
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):                 ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):           ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Nachschlage_Art'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Nachschlage_Art($a_return=0, $a_arg_array=array(), $s_feldname='Nachschlage_Art')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

	// Select-Argumente vorbereiten
	$s_name="SELECT mandanten_id, pname FROM personen WHERE personentyp=274 AND mandanten_id>0 ".db_orderby("mandanten_id");
	$s_func='$selectwerte["pname"]=$selectwerte["mandanten_id"]." - ".$selectwerte["pname"];';
	$s_wert='mandanten_id';
	$s_feld='pname';

	// Auswahl-Array erstellen
	$arg_array=argArray($s_name,$s_func,$s_feld,$s_wert,$s_feldname,$a_arg_array);
	$a_array=select_array($arg_array);
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):          ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):    ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'FxTab_ID'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_FxTab_ID($a_return=0, $a_arg_array=array(), $s_feldname='FxTab_ID')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

	// Select-Argumente vorbereiten
	$s_name="select tabname, id_tabellen from tabellen where tabelleart=2 or tabelleart=4 ".db_orderby("tabname");	// SELECT-SQL, kann auch bereits fertiges Array sein
	$s_func='';	// Funktion zum Ändern der Selectwerte
	$s_wert='id_tabellen';	// SELECT-WERT		Array-Aufbau:
	$s_feld='tabname';	// SELECT-FELD		$a_array[s_wert]="s_feld";

	// Auswahl-Array erstellen
	// oder selbst erstellen in o.g. Form inkl. Default-Wert mit Key 0
	$arg_array=argArray($s_name,$s_func,$s_feld,$s_wert,$s_feldname,$a_arg_array);
	$a_array=select_array($arg_array);
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):         ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):   ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'FeldTyp'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_FeldTyp($a_return=0, $a_arg_array=array(), $s_feldname='FeldTyp')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

	// Field types (without 99 = Integer (with Thousand Separator))
	$a_array=$GLOBALS['_ftypes'];
	unset($a[99]);

	// Do not display these field types?
	if($limit)
	{
		unset($a[2]);	// Serial
		unset($a[10]);	// Date + Time
		unset($a[13]);	// System
	}

	if($a_return == 1)
		return $a_array;

	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):            ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):      ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Anzeigeart'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Anzeigeart($a_return=0, $a_arg_array=array(), $s_feldname='Anzeigeart')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

	// Display types (without 0 = Undefined)
	$a_array=$GLOBALS['anzart_disptxt'];
	unset($a_array[0]);

	// Allow only certain display types for mask generator
	$limit=trim($limit);
	if(strlen($limit))
	{
		$ar=$a_array;
		$a_array=array();

		$la=explode(',', $limit);
		$aa=array();
		foreach($la as $lid)
			$aa[(int)$lid]=true;

		foreach($ar as $aid => $atx)
		{
			if(isset($aa[$aid]))
				$a_array[$aid]=$atx;
		}
	}

	if($a_return == 1)
		return $a_array;

	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):             ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):       ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Eingabefeld'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Eingabefeld($a_return=0, $a_arg_array=array(), $s_feldname='Eingabefeld')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

	// Einschränkungen treffen, außer wenn FeRox anlegt, dann sind immer alle Felder in Anzeige
	$zus='';
	if(!$all)
	{
		$z="mandanten_id=$Mandanten_ID";
		if($mode)
			$z .= " OR mandanten_id=0";
		$zus .= " AND ($z)";
	}

	// Einschränkungen anhand User-Angaben (=> Feldermenge einschränken)
	if($mode == 'Masken')
	{
		if($limit)
			$zus .= " AND id IN ($limit)";
		if($exclude)
			$zus .= " AND id NOT IN ($exclude)";
	}
	if($ausw_gen)
		$zus .= " AND feldtyp=5 AND nachschlage<>1";

	// Select-Argumente vorbereiten
	if($tabellen)
	{
		/* alle relevanten Felder aus den Tabellen "Projekte", "Zeitdaten" und "Budget" (ohne 120="Kunde" und 1454="Projektmanager") */
		$s_name='SELECT f.id, f.feldname, literal_20
					FROM felder f, texte t
					WHERE f.id=t.id_referenz
					AND t.id_ref_art=3
					AND t.id_sprache=1
					AND f.id IN (38, 44, 46, 47, 48, 69, 76, 110, 113, 121, 130, 132, 137, 297, 313, 349, 366, 373, 381, 1386, 1754, 1755, 1756, 1757, 1781, 1782)
					ORDER BY literal_20';
		$s_wert='feldname';
		$s_feld='literal_20';
	}
	else
	{
		$zus .= " AND gesperrt=0";
		$s_name="SELECT id, feldname FROM felder WHERE".substr($zus, 4)." ".db_orderby('feldname');
		$s_wert='id';
		$s_feld='feldname';
	}

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern (!)
	$arg_array=argArray($s_name,$s_func,$s_feld,$s_wert,$s_feldname,$a_arg_array);
	$a_array=select_array($arg_array);

	if($tabellen)
		$a_array=categorizeEingabefeld($a_array);

	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern (!)
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):                   ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):             ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Eingabefeld_alias'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Eingabefeld_alias($a_return=0, $a_arg_array=array(), $s_feldname='Eingabefeld_alias')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return auswahl_Eingabefeld($a_return, $a_arg_array, $s_feldname);
}

/**
 * kategorisiert das Auswahlfeld "Eingabefeld"
 * @param array $a_array das Auswahl-Array des Feldes "Eingabefeld"
 * @return array $a_array das Auswahl-Array des Feldes "Eingabefeld", nun mit eingefügten Kategorien
 */
function categorizeEingabefeld($a_array)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Namen der Kategorieren ermitteln
	$groups=get_array('SELECT wertetabid, tabwert FROM wertetab WHERE wertetabid IN (2676, 2677, 2678, 2679, 2680, 2681) AND id_sprache='.$GLOBALS['fxpglobals']['lang'], 'wertetabid', 'tabwert');
	$gid=2676;
	$feldZuordnung=array($groups[$gid++] => array('Name_Projekt', 'Projektart', 'Prioritaet', 'Projekt_Status', 'Elter'),
							$groups[$gid++] => array('Aufwand_Soll', 'Max_Zeit_Aufw'),
							$groups[$gid++] => array('Abrechnungsart', 'Abrechnungsart_Mat', 'budget_mat_ext_gepl', 'budget_pers_ext_gepl', 'budget_mat_int_gepl', 'budget_pers_int_gepl', 'budgetrahmen_mat_int', 'budgetrahmen_pers_int'),
							$groups[$gid++] => array('Ansprechpartner', 'Projektleiter', 'Vertreter'),
							$groups[$gid++] => array('Soll_End_Dtm', 'Soll_Beg_Dtm', 'Deadline'),
							$groups[$gid] => array('Abhaengigkeit', 'Anmerkung', 'AnmerkungIntern', 'Beschreibung', 'MaArt_ID', 'Vorgangsnummer'));
	if(is_array($feldZuordnung))
	{
		$a_arrayBCK=$a_array;
		unset($a_array);
		// Kategorien in Select-Array aufnehmen und DISABLED setzen
		foreach($feldZuordnung as $group => $elements)
		{
			$a_array[$group.'" DISABLED style="left: 10px; color: grey";']='* '.$group.' *';
			if(is_array($elements))
			{
				foreach($elements as $cnt => $element)
					$a_array[$element]=$a_arrayBCK[$element];
			}
		}
	}
	return $a_array;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):         ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):   ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'HorAusr'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_HorAusr($a_return=0, $a_arg_array=array(), $s_feldname='HorAusr')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

	// Select-Argumente vorbereiten
	$s_name="SELECT tabwert FROM wertetab WHERE id_feld=165 AND id_sprache=1 AND mandanten_id=0 ".db_orderby("position");
	$s_wert='tabwert';	// SELECT-WERT		Array-Aufbau:
	$s_feld='tabwert';	// SELECT-FELD		$a_array[s_wert]="s_feld"

	// Auswahl-Array erstellen
	$arg_array=argArray($s_name,$s_func,$s_feld,$s_wert,$s_feldname,$a_arg_array);
	$a_array=select_array($arg_array);
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):          ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):    ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'VertAusr'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_VertAusr($a_return=0, $a_arg_array=array(), $s_feldname='VertAusr')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

	// Select-Argumente vorbereiten
	$s_name="SELECT tabwert FROM wertetab WHERE id_feld=166 AND id_sprache=1 AND mandanten_id=0 ".db_orderby("position");
	$s_wert='tabwert';	// SELECT-WERT		Array-Aufbau:
	$s_feld='tabwert';	// SELECT-FELD		$a_array[s_wert]="s_feld"

	// Auswahl-Array erstellen
	$arg_array=argArray($s_name,$s_func,$s_feld,$s_wert,$s_feldname,$a_arg_array);
	$a_array=select_array($arg_array);
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):       ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()): ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Maske'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Maske($a_return=0, $a_arg_array=array(), $s_feldname='Maske')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];
	if($limit && !$all)
		$less="WHERE mandanten_id=".$Mandanten_ID;
	else
		$less="WHERE Name_Maske LIKE '_FG_%'";
	// Select-Argumente vorbereiten
	$s_name="SELECT nummer, name_maske FROM masken $less ".db_orderby("name_maske");	// SELECT-SQL, kann auch bereits fertiges Array sein
	$s_func='';			// Funktion zum Ändern der Selectwerte
	$s_wert='nummer';		// SELECT-WERT		Array-Aufbau:
	$s_feld='name_maske';	// SELECT-FELD		$a_array[s_wert]="s_feld";

	// Auswahl-Array erstellen
	$arg_array=argArray($s_name,$s_func,$s_feld,$s_wert,$s_feldname,$a_arg_array);
	$a_array=select_array($arg_array);
	if(is_array($a_array))
	{
		$mend='_'.$Mandanten_ID;
		$mlen=strlen($mend);
		foreach($a_array as $anr => $aval)
		{
			$nval=trim($aval);
			if(substr($nval, 0, 4) == '_FG_')
				$nval=substr($nval, 4);
			if(substr($nval, -$mlen) == $mend)
				$nval=substr($nval, 0, -$mlen);
			$a_array[$anr]=$nval;
		}
	}
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):                ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):          ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Projektmanager'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Projektmanager($a_return=0, $a_arg_array=array(), $s_feldname='Projektmanager')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return auswahl_Projektleiter($a_return, $a_arg_array, $s_feldname);
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):                ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):          ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Personentyp_ID'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Personentyp_ID($a_return=0, $a_arg_array=array(), $s_feldname='Personentyp_ID')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

	// Auswahl-Array erstellen
	$arg_array=argArray($s_name,$s_func,$s_feld,$s_wert,$s_feldname,$a_arg_array);
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):           ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):     ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Hauptmenu'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Hauptmenu($a_return=0, $a_arg_array=array(), $s_feldname='Hauptmenu')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Menüstruktur laden
	$mn=loadMenu();
	if(!sizeof($mn))
		return '';

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

	// Select-Argumente vorbereiten
	$i=0;
	foreach($mn as $ln => $le)
	{
		// Startknoten, alle Nicht-Ordner sowie Masken-Ordner überspringen
		if(($le['tr'] && !$whole) || ($le['special'] == 'mg'))
			continue;
		if(!$le['tr'])
			$le['tx']='<i>'.$le['tx'].'</i>';
		$s_name[$i]['eintrag']=str_repeat('&nbsp;',$le['lv']*5).$le['tx'];
		$s_name[$i]['node']=$ln;
		$i++;
	}

	$s_wert='node';
	$s_feld='eintrag';	// $a_array[s_wert]="s_feld";

	// Auswahl-Array erstellen
	$arg_array=argArray($s_name,$s_func,$s_feld,$s_wert,$s_feldname,$a_arg_array);
	$a_array=select_array($arg_array);
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):              ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):        ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'hauptmenu_id'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_hauptmenu_id($a_return=0, $a_arg_array=array(), $s_feldname='hauptmenu_id')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return auswahl_Hauptmenu($a_return, $a_arg_array, $s_feldname);
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):         ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):   ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Kontakt'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Kontakt($a_return=0, $a_arg_array=array(), $s_feldname='Kontakt')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

//*V|VD_LKZ|5|0|select w.wertetabid, w.id_sprache, w.tabwert, a.personen_id, w.id_feld, a.mandanten_id, a.adressen_id from adressen a, wertetab w where w.wertetabid=a.landkz and (w.mandanten_id=a.mandanten_id or w.mandanten_id=0)|fd7470199dacd7fe875221bc9c997f47
	// Select-Argumente vorbereiten
	$s_name ="SELECT p.personen_id, p.pname, p.vorname, w.tabwert AS lkz";
	if(($GLOBALS['prgclass'] < 40) && ($GLOBALS['fxpglobals']['persdat']['personentyp'] == 620))	// Ansprechpartner eines Kunden angemeldet
	{
		$p_kunde=db_value("SELECT elter FROM personen WHERE personen_id=".$GLOBALS['fxpglobals']['person']." AND mandanten_id=".Mandanten_ID);

		$s_name .= " FROM personen p LEFT JOIN vd_lkz w ON p.personen_id=w.personen_id AND p.mandanten_id=w.mandanten_id WHERE w.id_sprache=".$GLOBALS['fxpglobals']['lang'];
		$s_name .= " AND p.personen_id=".$p_kunde." AND p.mandanten_id=".$Mandanten_ID;
	}
	else if(($GLOBALS['prgclass'] < 40) && $pr_check)	// Nur projektspezifische Kunden
	{
		$s_name .= " FROM projekte pr, personen p LEFT JOIN vd_lkz w ON p.personen_id=w.personen_id AND p.mandanten_id=w.mandanten_id WHERE w.id_sprache=".$GLOBALS['fxpglobals']['lang'];
		$s_name .= " AND p.personentyp=1372 AND p.mandanten_id=".$Mandanten_ID." AND pr.mandanten_id=".$Mandanten_ID." AND pr.kunde=p.personen_id";
		if($GLOBALS['fxpglobals']['person'] && ($GLOBALS['fxpglobals']['user'] > 1) && !$GLOBALS['fxpglobals']['persdat']['roles'][2])
			$s_name .= " AND pr.projekt_id IN (".prights_get(0).")";
		$s_name .= " ".db_orderby('pname', 'vorname');
	}
	else
	{
		$s_name .= " FROM personen p LEFT JOIN vd_lkz w ON p.personen_id=w.personen_id AND p.mandanten_id=w.mandanten_id WHERE w.id_sprache=".$GLOBALS['fxpglobals']['lang'];
		$s_name .= " AND p.personentyp=1372 AND p.mandanten_id=".$Mandanten_ID;
		$s_name .= " ".db_orderby('pname', 'vorname');
	}
	$s_func='if($selectwerte["vorname"]) $selectwerte["pname"] .= ", ".$selectwerte["vorname"]; if($selectwerte["lkz"]) $selectwerte["pname"] .= " (".$selectwerte["lkz"].")";';
	$s_wert='p.personen_id';
	$s_feld='pname';

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$arg_array=argArray($s_name,$s_func,$s_feld,$s_wert,$s_feldname,$a_arg_array);
	$a_array=select_array($arg_array);
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):         ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):   ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'ID_Feld'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_ID_Feld($a_return=0, $a_arg_array=array(), $s_feldname='ID_Feld')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

	$sqlzusatz="";
	if($tab)
		$sqlzusatz .= "AND t.idintabellen=$tab ";
	if($typ)
		$sqlzusatz .= "AND f.feldtyp=$typ ";
	// Select-Argumente vorbereiten
	$s_name="SELECT f.id, f.feldname FROM felder f, tab_felder t WHERE f.id=t.idinfelder ".$sqlzusatz.db_orderby("feldname");
	$s_func='';			// Funktion zum Ändern der Selectwerte
	$s_wert='f.id';		// SELECT-WERT		Array-Aufbau:
	$s_feld='feldname';	// SELECT-FELD		$a_array[s_wert]="s_feld";

	// Auswahl-Array erstellen
	$arg_array=argArray($s_name,$s_func,$s_feld,$s_wert,$s_feldname,$a_arg_array);
	$a_array=select_array($arg_array);
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):         ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):   ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'ID_Text'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_ID_Text($a_return=0, $a_arg_array=array(), $s_feldname='ID_Text')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return auswahl_ID_Feld($a_return, $a_arg_array, $s_feldname);
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):         ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):   ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'TabName'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_TabName($a_return=0, $a_arg_array=array(), $s_feldname='TabName')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

	if($typ)
		$tab=" where tabelleart=$typ";
	// Select-Argumente vorbereiten
	$s_name="select id_tabellen, tabname from tabellen $tab ".db_orderby("tabname");	// SELECT-SQL, kann auch bereits fertiges Array sein
	$s_func='';	// Funktion zum Ändern der Selectwerte
	$s_wert='id_tabellen';	// SELECT-WERT		Array-Aufbau:
	$s_feld='tabname';	// SELECT-FELD		$a_array[s_wert]="s_feld";

	// Auswahl-Array erstellen
	$arg_array=argArray($s_name,$s_func,$s_feld,$s_wert,$s_feldname,$a_arg_array);
	$a_array=select_array($arg_array);
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):            ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):      ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'ID_Sprache'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_ID_Sprache($a_return=0, $a_arg_array=array(), $s_feldname='ID_Sprache')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

	// Select-Argumente vorbereiten
	$s_name="SELECT WerteTabID, TabWert FROM WerteTab WHERE ID_Feld=53 AND ID_Sprache=".$GLOBALS['fxpglobals']['lang']." ORDER BY TabWert";
	$s_func='';					// Funktion zum Ändern der Selectwerte
	$s_wert='wertetabid';		// SELECT-WERT		Array-Aufbau:
	$s_feld='tabwert';			// SELECT-FELD		$a_array[s_wert]="s_feld";

	// Auswahl-Array erstellen
	$arg_array=argArray($s_name,$s_func,$s_feld,$s_wert,$s_feldname,$a_arg_array);
	$a_array=select_array($arg_array);
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):         ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):   ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Sprache'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Sprache($a_return=0, $a_arg_array=array(), $s_feldname='Sprache')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

	// All languages
	$la=fxGetAllLanguages();
	$a_array=array();
	foreach($la as $lid => $a)
		$a_array[$lid]=$a['tx'];

	if($a_return == 1)
		return $a_array;

	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):           ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):     ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Termin_ID'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Termin_ID($a_return=0, $a_arg_array=array(), $s_feldname='Termin_ID')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

	if(!isset($override))
	{
		$s_wert='termin_id';	// SELECT-WERT		Array-Aufbau:
		$s_feld='name_termin';	// SELECT-FELD		$a_array[s_wert]="s_feld";

		// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern (!)
		$arg_array=argArray($s_name,$s_func,$s_feld,$s_wert,$s_feldname,$a_arg_array);
		$a_array=array();

		// Rollengruppe einer Person sowie die Termine, die diese Person sehen darf ermitteln
		if(!is_array($gpa))
			$gpa=get_gpa($GLOBALS['fxpglobals']['person'], true, true, $tart);
//fxDebug($gpa, '$gpa');
		if(fxIsArray($gpa) && is_array($gpa['app']))
		{
			// Filter setzen
			$GLOBALS['mskfilter'] |= $gpa['flt'];

			$erl=true;

			// Hat die Person einen Vorgesetzten?
			$elt=get_realelter();
			// Gibt es GF?
			$gf=(int)db_value("SELECT personen_id FROM ma_art_faehigkeiten WHERE maart_id=1 AND mafaehigkeit_id=2 AND mandanten_id=".$Mandanten_ID);
			// Gibt es VW?
			$vw=(int)db_value("SELECT personen_id FROM ma_art_faehigkeiten WHERE maart_id=1 AND mafaehigkeit_id=-1 AND mandanten_id=".$Mandanten_ID);

			// Terminarten
			$tarten=array();

			// Terminarray durchgehen
			$pbck=0;
			$today=fxNow(false).'240000';
			foreach($gpa['app'] as $id => $a)
			{
				$pid=$a['personen_id'];
				$par=$gpa['par'][$pid];
				$pgr=$gpa['pgr'][$pid];

				// Soll der Termin angezeigt werden?
				$show=0;
				// nur, falls Termin nocht nicht erledigt oder erledigte Termine angezeigt werden sollen
				if(!$a['erledigt'] || $erl)
				{
					// für Transaktion 222: Abwesenheitsplaner
					if($GLOBALS['fxpglobals']['tr'] == 222)
					{
						// FXP_DISPLAY:
						// Termin liegt im Zeitraum
						if(($a['startzeit'] <= $gpa['ed']) && ($a['endzeit'] >= $gpa['sd']))
							$show |= get_bit(FXP_DISPLAY);
						// FXP_CHANGE:
						// Termin liegt im Zeitraum
						if(($a['startzeit'] <= $gpa['ed']) && ($a['endzeit'] >= $gpa['sd']))
						{
							// Person ist SA, GF oder VW ohne GF
							if(($pgr & FXP_BAR_SYSADMIN) || ($pgr & FXP_BAR_EXECUTIVE) || (($pgr & FXP_BAR_MANAGEMENT) && !$gf))
								$show |= get_bit(FXP_CHANGE);
							// Person ist VW oder VG (von nicht eigenen Terminen)
							else if(($pid != $GLOBALS['fxpglobals']['person']) && (($pgr & FXP_BAR_MANAGEMENT) || ($pgr & FXP_BAR_SUPERVISOR)))
								$show |= get_bit(FXP_CHANGE);
							// Person hat keinen VG und es gibt weder GF noch VW
							else if(!$elt && !$gf && !$vw)
								$show |= get_bit(FXP_CHANGE);
							// Nicht bestätigte Termine
							else if(!$a['hat_bestaetigt'])
								$show |= get_bit(FXP_CHANGE);
						}
						// FXP_DELETE:
						// Person ist SA, GF oder VW ohne GF
						if(($pgr & FXP_BAR_SYSADMIN) || ($pgr & FXP_BAR_EXECUTIVE) || (($pgr & FXP_BAR_MANAGEMENT) && !$gf))
							$show |= get_bit(FXP_DELETE);
						// Person ist VW oder VG (von nicht eigenen Terminen)
						else if(($pid != $GLOBALS['fxpglobals']['person']) && (($pgr & FXP_BAR_MANAGEMENT) || ($pgr & FXP_BAR_SUPERVISOR)))
							$show |= get_bit(FXP_DELETE);
						// Person hat keinen VG und es gibt weder GF noch VW
						else if(!$elt && !$gf && !$vw)
							$show |= get_bit(FXP_DELETE);
						// Nicht bestätigte Termine
						else if(!$a['hat_bestaetigt'])
							$show |= get_bit(FXP_DELETE);
					}
					// sonstige
					else
					{
						// FXP_DISPLAY:
						// Termin liegt im Zeitraum
						if(($a['startzeit'] <= $gpa['ed']) && ($a['endzeit'] >= $gpa['sd']))
							$show |= get_bit(FXP_DISPLAY);
						// FXP_CHANGE:
						// Termin liegt im Zeitraum
						if(($a['startzeit'] <= $gpa['ed']) && ($a['endzeit'] >= $gpa['sd']))
						{
							// Eigene Person involviert
							if(($a['personen_id'] == $GLOBALS['fxpglobals']['person']) || ($a['ansprechpartner'] == $GLOBALS['fxpglobals']['person']) || ($a['personen_angelegt'] == $GLOBALS['fxpglobals']['person']))
								$show |= get_bit(FXP_CHANGE);
							// Person hat keinen Vorgesetzten oder bestimmte Rollengruppenrechte
							else if(!$elt || ($pgr & FXP_BAR_SYSADMIN) || ($pgr & FXP_BAR_EXECUTIVE) || ($pgr & FXP_BAR_MANAGEMENT))
								$show |= get_bit(FXP_CHANGE);
							// Fremde Privattermine immer ausblenden, d.h. Ändern-Bit löschen
							if(($a['masterterminart'] == FXP_AT_PRIVATE) && ($a['personen_id'] != $GLOBALS['fxpglobals']['person']))
								$show &= 65535-get_bit(FXP_CHANGE);
						}
						// FXP_DELETE:
						// Termin liegt im Zeitraum
						if(($a['startzeit'] <= $gpa['ed']) && ($a['endzeit'] >= $gpa['sd']))
						{
							// Eigene Person involviert
							if(($a['personen_id'] == $GLOBALS['fxpglobals']['person']) || ($a['personen_angelegt'] == $GLOBALS['fxpglobals']['person']))
								$show |= get_bit(FXP_DELETE);
							// Nicht bestätigte Termine
							else if(!$a['hat_bestaetigt'])
								$show |= get_bit(FXP_DELETE);
							// Person hat keinen Vorgesetzten oder bestimmte Rollengruppenrechte
							else if(!$elt || ($pgr & FXP_BAR_SYSADMIN) || ($pgr & FXP_BAR_EXECUTIVE) || ($pgr & FXP_BAR_MANAGEMENT))
								$show |= get_bit(FXP_DELETE);
							// Fremde Privattermine immer ausblenden, d.h. Ändern-Bit löschen
							if(($a['masterterminart'] == FXP_AT_PRIVATE) && ($a['personen_id'] != $GLOBALS['fxpglobals']['person']))
								$show &= 65535-get_bit(FXP_DELETE);
						}
					}
				}

				// Übermittelte Termine immer anzeigen
				if($_GET['Termin_ID'] && ($a['termin_id'] == $_GET['Termin_ID']))
					$show |= get_bit($GLOBALS['aktion']);
				if(!test_bit($show, $GLOBALS['aktion']))
					continue;

				// Name anzeigen, falls Person einer Rollengruppe angehört
				if($gpa['grp'] > FXP_BAR_SELF)
				{
					$name=$a['pname'];
					$vname=trim($a['vorname']);
					if(strlen($vname))
						$name .= ', '.$vname;

					if($pbck != $pid)
					{
						if($pbck)
							$a_array['_t'.$pid]='<d>';
						if($a['personen_id'] == $GLOBALS['fxpglobals']['person'])
							$fw='<blue>'.$name.'</blue>';
						else
							$fw=$name;
						$a_array['_n'.$pid]='<b>'.$fw.'</b><bb><bg#e0e0e0>';
					}
					$pbck=$pid;
				}

				$aid=$id;

				// Zeitraum ermitteln
				$zeit=get_entry($a['startzeit'], 'datetime');
				if($a['endzeit'] && ($a['startzeit'] != $a['endzeit']))
				{
					if(substr($a['startzeit'], 0, 8) == substr($a['endzeit'], 0, 8))	// gleicher Tag
						$zeit .= ' - '.get_entry($a['endzeit'], 'zeit');
					else
						$zeit .= ' - '.get_entry($a['endzeit'], 'datetime');
				}
				$a_array[$aid]=$zeit;
				// Terminart anzeigen?
				$show_ta=false;
				// ja, falls eigene Person involviert
				if(($a['personen_id'] == $GLOBALS['fxpglobals']['person']) || ($a['ansprechpartner'] == $GLOBALS['fxpglobals']['person']) || ($a['personen_angelegt'] == $GLOBALS['fxpglobals']['person']))
					$show_ta=true;
				// ja, falls Recht auf Ändern und kein Privattermin
				else if(test_bit($par, FXP_CHANGE) && ($a['masterterminart'] != FXP_AT_PRIVATE))
					$show_ta=true;
				// Nur bei eigener Person auch Text im Auswahlfeld anzeigen, sonst nur Zeitpunkt
				if($show_ta)
				{
					if(!$tarten[$a['terminart']])
						$tarten[$a['terminart']]=meldung($a['terminart'], true);
					$a_array[$aid] .= ': '.$tarten[$a['terminart']];
					$fld=trim($a[$s_feld]);
					if(strlen($fld) && ($fld != $tarten[$a['terminart']]))
						$a_array[$aid] .= ' ('.$fld.')';
				}
				// Wiederholungstermin?
				if($a['wiederholung'])
					$a_array[$aid] .= ' {+}';
				// Erinnerungstermin?
				else if($a['terminart'] == 180)
					$a_array[$aid] .= ' {!}';

				if($a['erledigt'])
					$a_array[$aid]='!..'.$a_array[$aid];
				if(!test_bit($show, FXP_CHANGE) || !test_bit($par, FXP_CHANGE))
					$a_array[$aid]='..('.$a_array[$aid].')';
			}
//fxDebug($a_array, '$a_array');
		}
	}
	else
		$a_array=$override;
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern (!)
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):             ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):       ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Fehltagsart'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Fehltagsart($a_return=0, $a_arg_array=array(), $s_feldname='Fehltagsart')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

	// Select-Argumente vorbereiten
	$s_name ="SELECT wertetabid, tabwert FROM wertetab";
	$s_name .= " WHERE id_feld=434 AND mandanten_id=0 AND id_sprache=".$GLOBALS['fxpglobals']['lang'];
	if(strlen($tart))
		$s_name .= " AND wertetabid IN (".$tart.")";
	else if($s_feldname == 'Fehltagsart')
		$s_name .= " AND wertetabid IN (".FXP_AT_VACATIONREQUEST.",".FXP_AT_SPECIALVACATIONREQUEST.",".FXP_AT_SICKCALL.",".FXP_AT_TRAININGREQUEST.",".FXP_AT_SLIDINGDAYREQUEST.",".FXP_AT_OVERTIMEOUTPAYREQUEST.",".FXP_AT_ABSENT.",".FXP_AT_VACATION.",".FXP_AT_SPECIALVACATION.",".FXP_AT_SICK.",".FXP_AT_TRAINING.",".FXP_AT_SLIDINGDAY.",".FXP_AT_OVERTIMEOUTPAY.")";
	else
		$s_name .= " AND wertetabid IN (".FXP_AT_MEETING.",".FXP_AT_CONFERENCE.",".FXP_AT_BUSINESSDINNER.",".FXP_AT_CONTACT.",".FXP_AT_PRIVATE.",".FXP_AT_MESSAGE.",".FXP_AT_VARIOUS.")";
	$s_name .= " ORDER BY tabwert";
	$s_wert='wertetabid';	// SELECT-WERT		Array-Aufbau:
	$s_feld='tabwert';	// SELECT-FELD		$a_array[s_wert]="s_feld";

	// Auswahl-Array erstellen
	$arg_array=argArray($s_name,$s_func,$s_feld,$s_wert,$s_feldname,$a_arg_array);
	$a_array=select_array($arg_array);
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):           ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):     ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Terminart'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Terminart($a_return=0, $a_arg_array=array(), $s_feldname='Terminart')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return auswahl_Fehltagsart($a_return, $a_arg_array, $s_feldname);
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):              ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):        ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Vorgesetzter'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Vorgesetzter($a_return=0, $a_arg_array=array(), $s_feldname='Vorgesetzter')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

	// Select-Argumente vorbereiten
	$s_name ="SELECT p.personen_id, p.pname, p.vorname FROM personen p, personen e";
	$s_name .= " WHERE p.personen_id=e.elter AND (p.inaktiv=0 OR p.inaktiv IS NULL) AND e.elter>0 AND p.personentyp=275 AND e.mandanten_id=$Mandanten_ID";
	$s_name .= " ORDER BY p.pname, p.vorname";
	$s_func='if(strlen(trim($selectwerte["vorname"]))) $selectwerte["pname"] .= ", ".trim($selectwerte["vorname"]);';
	$s_wert='personen_id';	// SELECT-WERT		Array-Aufbau:
	$s_feld='pname';			// SELECT-FELD		$a_array[s_wert]="s_feld";

	// Auswahl-Array erstellen
	$arg_array=argArray($s_name,$s_func,$s_feld,$s_wert,$s_feldname,$a_arg_array);
	$a_array=select_array($arg_array);
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):           ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):     ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'RepViewID'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_RepViewID($a_return=0, $a_arg_array=array(), $s_feldname='RepViewID')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

	// Select-Argumente vorbereiten
	$s_name="SELECT mandanten_id, repviewid, repviewname, name_wert FROM repviews WHERE (mandanten_id=0 OR mandanten_id=".$Mandanten_ID.") ORDER BY repviewid";
	$s_func='if($selectwerte["repviewname"]) $swrn=meldung($selectwerte["repviewname"], true); else $swrn=""; if($swrn) $selectwerte["name_wert"]=$swrn; if(!strlen($selectwerte["name_wert"])) $selectwerte["name_wert"]=$selectwerte["repviewid"]; $selectwerte["name_wert"]="V_FXP_".substr("0000".$selectwerte["repviewid"], -4)." - ".$selectwerte["name_wert"]; if($selectwerte["mandanten_id"]) $selectwerte["name_wert"] .= " [".$selectwerte["mandanten_id"]."]";';
	$s_wert='repviewid';	// SELECT-WERT		Array-Aufbau:
	$s_feld='name_wert';	// SELECT-FELD		$a_array[s_wert]="s_feld";

	// Auswahl-Array erstellen
	$arg_array=argArray($s_name,$s_func,$s_feld,$s_wert,$s_feldname,$a_arg_array);
	$a_array=select_array($arg_array);
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):               ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):         ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Sachmittel_ID'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Sachmittel_ID($a_return=0, $a_arg_array=array(), $s_feldname='Sachmittel_ID')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

	// Auswahl-Array erstellen
	if(isset($override))
		$a_array=$override;
	else
	{
		$sql  = "SELECT s.sachmittel_id, s.verwendungsart, s.sachmitteltyp, s.artikelnummer_int, s.artikelnummer_ext, s.name_sachmittel, b.bestand_akt, b.bestand_verbr, b.bestand_zug";
		$sql .= " FROM sachmittel s LEFT JOIN materialbestand b ON (b.sachmittel_id=s.sachmittel_id AND b.mandanten_id=s.mandanten_id AND b.bestand_akt IS NOT NULL)";
		$sql .= " WHERE s.mandanten_id=".$Mandanten_ID;
		$where="";
		// ...consider selected number and name criteria
		if(strlen($inumberi))
			$where .= " AND ".db_like('s.artikelnummer_int', '%'.$inumberi.'%');
		if(strlen($name))
			$where .= " AND ".db_like('s.name_sachmittel', '%'.$name.'%');
		// ...consider selected project criteria
		if(fxIsArray($arr_prj))
		{
//fxDebug($arr_prj,'$arr_prj');
			$csv_prj='';
			foreach($arr_prj as $cpid => $pn)
			{
				$upos=strpos($cpid, '_');	// Leistungsart
				$mpos=strpos($cpid, '-');	// ToDo
				if(($upos === false) && ($mpos === false))
				{
					$pid=(int)$cpid;
					if((substr($pn,-2) != '*}') && ($pid > 0))
						$csv_prj .= ", ".$pid;
				}
			}
			if(strlen($csv_prj))
				$where .= " AND s.fuervorgang IN (".substr($csv_prj,2).", 0)";
			if($project)
			{
				$pn=db_value("SELECT vorgangsnummer FROM projekte WHERE projekt_id=".$project." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
				$where .= " AND s.fuervorgang IN (SELECT projekt_id FROM projekte WHERE vorgangsnummer LIKE '".$pn."%' AND mandanten_id=".$GLOBALS['fxpglobals']['client'].")";
			}
		}
		// ...consider material type criteria
		if(fxIsArray($mtypes) && ($mtypes[0] > 0))
			$where .= " AND s.sachmitteltyp IN (".array_to_csv($mtypes,false,true).")";
		// ...consider stock location criteria
		$sl='';
		if(fxIsArray($slocations) && ($slocations[0] != 0))
		{
			foreach($slocations as $sloc)
			{
				if(strlen($sl))
					$sl .= " OR ";
				$sl .= db_like('s.lagerort',$sloc);
			}
		}
		if(strlen($sl))
			$where .= " AND (s.verwendungsart=3151 OR (s.verwendungsart=3150 AND (".$sl.")))";
		// ...consider usage category criteria
		if(!isset($uccsv))
			$uccsv='';
		if(fxIsArray($ucats))
		{
			foreach($ucats as $ucid => $ucval)
			{
				if($ucval)
				{
					if(strlen($uccsv))
						$uccsv .= ',';
					$uccsv .= $ucid;
				}
			}
		}
		if(strlen($uccsv))
			$where .= " AND s.verwendungsart IN (".$uccsv.")";
		// ...consider selected timespan criteria
		if(strlen($sdate) && strlen($edate))
			$where .= " AND ((s.soll_beginn IS NULL AND s.soll_ende IS NULL) OR (s.soll_beginn<='".substr($edate,0,8)."240000' AND (s.soll_ende IS NULL OR s.soll_ende>='".substr($sdate,0,8)."000000')))";
		else if(strlen($sdate))
			$where .= " AND (s.soll_beginn IS NULL OR s.soll_beginn>='".substr($sdate,0,8)."000000')";
		else if(strlen($edate))
			$where .= " AND (s.soll_ende IS NULL OR s.soll_ende<='".substr($edate,0,8)."240000')";

		$sql .= $where." ORDER BY s.verwendungsart, s.artikelnummer_int, s.name_sachmittel";
		$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql, 0);

		$a_array=array();
		if(fxIsArray($tmp))
		{
			$st=sizeof($tmp);
			$mps=getSettingValue('max_pic_sel');
			$sl=get_text(2027,'10');	// Stock
			$ut=0;
			$ec=-1;
			$pf='&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
			foreach($tmp as $t)
			{
				if($t['verwendungsart'] != $ut)
				{
					$ut=$t['verwendungsart'];
					if(sizeof($a_array))
					{
						$a_array[$ec]='<d>';
						$ec--;
					}
					$a_array[$ec]=meldung($ut,false).'<d>';
					$ec--;
				}

				$id=$t['verwendungsart'].'_'.$t['sachmittel_id'];
				$pp='';
				if(!$mps || ($st < $mps))
				{
					$mmxh=256;
					$mia=getMaterialIcons($t['sachmittel_id'],$mmxh);
//fxDebug($mia,'$mia: '.$t['sachmittel_id']);
					if(strlen($mia['ico']))
					{
						if($mia['def'])
							$pp='<img src='.$mia['ico'].$mia['mtime'].' align=top width=13 height=13 style=\'border:1px solid #d2d2d2;border-radius:2px;opacity:0.10;\' />&nbsp;';
						else
							$pp='<tt><img src='.$mia['256'].$mia['mtime'].$mia['size_256'].' style=\'max-height:'.$mmxh.'px;width:auto;margin-top:4px;\' /></tt><img src='.$mia['ico'].$mia['mtime'].' align=top width=13 height=13 style=\'border:1px solid #d2d2d2;border-radius:2px;\' />&nbsp;';
					}
				}
				if(strlen($t['artikelnummer_int']))
					$a_array[$id]=$pf.$pp.'['.$t['artikelnummer_int'].']&nbsp;=&nbsp;'.$t['name_sachmittel'];
				else
					$a_array[$id]=$pf.$pp.$t['name_sachmittel'];
				if(strlen($t['artikelnummer_ext']))
					$a_array[$id] .= '&nbsp;&nbsp;&nbsp;{'.$t['artikelnummer_ext'].'}';
				if(($t['verwendungsart'] == 3150) && strlen($t['bestand_akt']))	// Usage Material
					$a_array[$id] .= '&nbsp;&nbsp;&nbsp;('.$sl.': '.get_entry($t['bestand_akt'],'dezimal').')';
			}
		}
	}
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):             ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):       ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'auftrags_id'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_auftrags_id($a_return=0, $a_arg_array=array(), $s_feldname='auftrags_id')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

	$GLOBALS['mskfilter'] |= 512;

	// Select-Argumente vorbereiten
	$s_name="SELECT auftrags_id, auftragstitel, projekt_id FROM auftrag";
	$s_name .= " WHERE mandanten_id=".$Mandanten_ID;

	// FILTER: Auftraggeber
	if($GLOBALS['fxpglobals']['setfilter'] && !$GLOBALS['fxpglobals']['filter']['fprojekt_inaktiv'] && is_array($GLOBALS['fxpglobals']['filter']['kunde']) && ($GLOBALS['fxpglobals']['filter']['kunde'][0] != 0))
	{
		$GLOBALS['mskfilter'] |= 320;
		$ids=implode(',', $GLOBALS['fxpglobals']['filter']['kunde']);
		$s_name .= " AND kunde IN (".$ids.")";
	}
	// Ist ein Kunde eingeloggt, dann ebenfalls Auswahl auf diesen Kunden eingrenzen
	if(($GLOBALS['prgclass'] < 40) && ($GLOBALS['fxpglobals']['persdat']['personentyp'] == 620))
	{
		$p_kunde=db_value("SELECT elter FROM personen WHERE personen_id=".$GLOBALS['fxpglobals']['person']);
		$s_name .= " AND kunde=".$p_kunde;
	}
	// FILTER: Suchtext
	if($GLOBALS['fxpglobals']['setfilter'] && !$GLOBALS['fxpglobals']['filter']['fprojekt_inaktiv'] && strlen($GLOBALS['fxpglobals']['filter']['projektname']))
	{
		$GLOBALS['mskfilter'] |= 320;
		$s_name .= " AND ".db_like('auftragstitel', '%'.$GLOBALS['fxpglobals']['filter']['projektname']."%");
	}
	// FILTER: Projekt vom - Projekt bis
	if($GLOBALS['fxpglobals']['setfilter'] && !$GLOBALS['fxpglobals']['filter']['fprojekt_inaktiv'] && (strlen($GLOBALS['fxpglobals']['filter']['proj_menu_von']) || strlen($GLOBALS['fxpglobals']['filter']['proj_menu_bis'])))
	{
		$GLOBALS['mskfilter'] |= 320;
		if(strlen($GLOBALS['fxpglobals']['filter']['proj_menu_von']))
			$s_name .= " AND fertigtermin>='".substr($GLOBALS['fxpglobals']['filter']['proj_menu_von'], 0, 8)."000000'";
		if(strlen($GLOBALS['fxpglobals']['filter']['proj_menu_bis']))
			$s_name .= " AND fertigtermin<='".substr($GLOBALS['fxpglobals']['filter']['proj_menu_bis'], 0, 8)."240000'";
	}
	// FILTER: Priorität
	if($GLOBALS['fxpglobals']['setfilter'] && !$GLOBALS['fxpglobals']['filter']['fprojekt_inaktiv'] && is_array($GLOBALS['fxpglobals']['filter']))
	{
		$tmp='';
		$cnt=0;
		foreach($GLOBALS['fxpglobals']['filter'] as $feld => $wert)
		{
			if((substr($feld, 0, 11) == 'prioritaet_') && $wert)
			{
				$tmp .= " OR auftragsprio=".(int)substr($feld, 11);
				$cnt++;
			}
		}
		if($cnt && ($cnt < 4))	// Bei 4 Einträgen wurde alles angehakt => Einschränkung nicht nötig
		{
			$GLOBALS['mskfilter'] |= 320;
			$s_name .= " AND (".substr($tmp, 4).")";
		}
	}

	$s_addw='';
	// FILTER: Mutterprojekt
	if($GLOBALS['fxpglobals']['setfilter'] && !$GLOBALS['fxpglobals']['filter']['fprojekt_inaktiv'] && is_array($GLOBALS['fxpglobals']['filter']['mutterprojekt_id']) && ($GLOBALS['fxpglobals']['filter']['mutterprojekt_id'][0] != 0))
	{
		$ids=implode(',', $GLOBALS['fxpglobals']['filter']['mutterprojekt_id']);
		$s_addw .= " AND mutterprojekt_id IN (".$ids.")";
	}
	// FILTER: Projektmanager
	if($GLOBALS['fxpglobals']['setfilter'] && !$GLOBALS['fxpglobals']['filter']['fprojekt_inaktiv'] && is_array($GLOBALS['fxpglobals']['filter']['projektmanager']) && ($GLOBALS['fxpglobals']['filter']['projektmanager'][0] != 0))
	{
		$ids=implode(',', $GLOBALS['fxpglobals']['filter']['projektmanager']);
		$s_addw .= " AND projektmanager IN (".$ids.")";
	}
	// FILTER: Fähigkeit
	if($GLOBALS['fxpglobals']['setfilter'] && !$GLOBALS['fxpglobals']['filter']['fprojekt_inaktiv'] && is_array($GLOBALS['fxpglobals']['filter']['mafaehigkeit_id']) && ($GLOBALS['fxpglobals']['filter']['mafaehigkeit_id'][0] != 0))
	{
		$ida='';
		$idf='';
		foreach($GLOBALS['fxpglobals']['filter']['mafaehigkeit_id'] as $f)
		{
			if(substr($f, -1) == 'f')
				$idf .= ','.(int)$f;
			else
				$ida .= ','.(int)$f;
		}
		$zus='';
		if($ida)
			$zus .= " OR maart_id IN (".substr($ida, 1).")";
		if($idf)
			$zus .= " OR mafaehigkeit_id IN (".substr($idf, 1).")";
		$s_addw .= " AND (".substr($zus, 4).")";
	}
	// FILTER: Projektleiter
	if($GLOBALS['fxpglobals']['setfilter'] && !$GLOBALS['fxpglobals']['filter']['fprojekt_inaktiv'] && is_array($GLOBALS['fxpglobals']['filter']['projektleiter']) && ($GLOBALS['fxpglobals']['filter']['projektleiter'][0] != 0))
	{
		$ids=implode(',', $GLOBALS['fxpglobals']['filter']['projektleiter']);
		$s_addw .= " AND (projektleiter IN (".$ids.") OR vertreter IN (".$ids."))";
	}
	// Projekt-Kategorie
	if($GLOBALS['fxpglobals']['setfilter'] && !$GLOBALS['fxpglobals']['filter']['fprojekt_inaktiv'] && is_array($GLOBALS['fxpglobals']['filter']['kategorie_id']) && ($GLOBALS['fxpglobals']['filter']['kategorie_id'][0] != 0))
	{
		$ids=implode(',', $GLOBALS['fxpglobals']['filter']['kategorie_id']);
		$kids=pk_ids($ids);
		$s_addw .= " AND kategorie_id IN (".$kids.")";
	}
	// Projekt-Status
	if($GLOBALS['fxpglobals']['setfilter'] && !$GLOBALS['fxpglobals']['filter']['fprojekt_inaktiv'] && is_array($GLOBALS['fxpglobals']['filter']))
	{
		$tmp='';
		$cnt=0;
		foreach($GLOBALS['fxpglobals']['filter'] as $feld => $wert)
		{
			if((substr($feld, 0, 14) == 'projektstatus_') && $wert)
			{
				$tmp .= " OR projekt_status=".(int)substr($feld, 14);
				$cnt++;
			}
		}
		if($cnt < 4 && $cnt)	// Bei 4 Einträgen wurde alles angehakt => Einschränkung nicht nötig
			$s_addw .= " AND (".substr($tmp, 4).")";
	}
	if(strlen($s_addw))
	{
		$GLOBALS['mskfilter'] |= 320;
		$idsql="SELECT projekt_id FROM projekte WHERE projektart=".FXP_PRJ_MAIN." AND mandanten_id=".$Mandanten_ID.$s_addw;
		$chkid=(int)db_value($idsql);
		// FILTER: Nur Auftragsv.
		if($GLOBALS['fxpglobals']['setfilter'] && !$GLOBALS['fxpglobals']['filter']['fprojekt_inaktiv'] && $GLOBALS['fxpglobals']['filter']['nurauftrag'])
		{
			$s_name .= " AND (projekt_id IS NOT NULL OR projekt_id>0)";
			if($chkid)
				$s_name .= " AND projekt_id IN (".$idsql.")";
		}
		else if($chkid)
			$s_name .= " AND ((projekt_id IS NULL OR projekt_id<1) OR (projekt_id IN (".$idsql.")))";
		else
			$s_name .= " AND (projekt_id IS NULL OR projekt_id<1)";
	}
	else if($GLOBALS['fxpglobals']['setfilter'] && !$GLOBALS['fxpglobals']['filter']['fprojekt_inaktiv'] && $GLOBALS['fxpglobals']['filter']['nurauftrag'])
	{
		$GLOBALS['mskfilter'] |= 320;
		$s_name .= " AND (projekt_id IS NOT NULL OR projekt_id>0)";
	}
	$s_name .= " ORDER BY auftragstitel";
	$s_func='if($selectwerte["projekt_id"] > 0) $selectwerte["auftragstitel"] .= " [*]";';
	$s_wert='auftrags_id';	// SELECT-WERT		Array-Aufbau:
	$s_feld='auftragstitel';	// SELECT-FELD		$a_array[s_wert]="s_feld";

	// Auswahl-Array erstellen
	$arg_array=argArray($s_name,$s_func,$s_feld,$s_wert,$s_feldname,$a_arg_array);
	$a_array=select_array($arg_array);
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):                 ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):           ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'kostenstelle_id'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_kostenstelle_id($a_return=0, $a_arg_array=array(), $s_feldname='kostenstelle_id')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];
	if(!strlen($kst_gueltig_von))
		$kst_gueltig_von='00000000000000';
	if(!strlen($kst_gueltig_bis))
		$kst_gueltig_bis='99999999999999';
	if(!$kst_def)
		$kst_def='900,901,902,903';

	// Select-Argumente vorbereiten
	$s_name="SELECT kostenstelle_id, kostennummer, kostenbezeichnung FROM kostenstellen";
	$s_name .= " WHERE mandanten_id=$Mandanten_ID AND id_sprache=".$GLOBALS['fxpglobals']['lang'];
	$s_name .= " AND kostendefinition IN ($kst_def)";
	if($kst_id)
		$s_name .= " AND (kostenstelle_id=$kst_id OR (";
	$s_zus  ="gueltig_von<='$kst_gueltig_bis' AND gueltig_bis>='$kst_gueltig_von'";
	if($kst_art)
		$s_zus .= " AND (kostenstellenart IS NULL OR kostenstellenart IN ($kst_art))";
	if($kst_id)
		$s_name .= $s_zus."))";
	else
		$s_name .= " AND ".$s_zus;
	$s_name .= " ORDER BY kostennummer";
	$s_func='if(strlen(trim($selectwerte["kostenbezeichnung"]))) $selectwerte["kostennummer"] .= ": ".$selectwerte["kostenbezeichnung"];';
	$s_wert='kostenstelle_id';	// SELECT-WERT		Array-Aufbau:
	$s_feld='kostennummer';		// SELECT-FELD		$a_array[s_wert]="s_feld";

	// Auswahl-Array erstellen
	$arg_array=argArray($s_name,$s_func,$s_feld,$s_wert,$s_feldname,$a_arg_array);
	$a_array=select_array($arg_array);
//fxDebug($a_array, '<B>'.$s_feldname.'</B>:<br />'.$s_name);
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):               ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):         ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'kostentraeger'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_kostentraeger($a_return=0, $a_arg_array=array(), $s_feldname='kostentraeger')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!is_array($GLOBALS[$s_feldname.'_arg']) || !strlen($GLOBALS[$s_feldname.'_arg']['kst_def']))
		$GLOBALS[$s_feldname.'_arg']['kst_def']='900';

	return auswahl_kostenstelle_id($a_return, $a_arg_array, $s_feldname);
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):               ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):         ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'kostenstellen'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_kostenstellen($a_return=0, $a_arg_array=array(), $s_feldname='kostenstellen')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!is_array($GLOBALS[$s_feldname.'_arg']) || !strlen($GLOBALS[$s_feldname.'_arg']['kst_def']))
		$GLOBALS[$s_feldname.'_arg']['kst_def']='901';

	return auswahl_kostenstelle_id($a_return, $a_arg_array, $s_feldname);
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):             ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):       ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'kostenarten'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_kostenarten($a_return=0, $a_arg_array=array(), $s_feldname='kostenarten')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!is_array($GLOBALS[$s_feldname.'_arg']) || !strlen($GLOBALS[$s_feldname.'_arg']['kst_def']))
		$GLOBALS[$s_feldname.'_arg']['kst_def']='902';

	return auswahl_kostenstelle_id($a_return, $a_arg_array, $s_feldname);
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):                ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):          ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'leistungsarten'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_leistungsarten($a_return=0, $a_arg_array=array(), $s_feldname='leistungsarten')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!is_array($GLOBALS[$s_feldname.'_arg']) || !strlen($GLOBALS[$s_feldname.'_arg']['kst_def']))
		$GLOBALS[$s_feldname.'_arg']['kst_def']='903';

	return auswahl_kostenstelle_id($a_return, $a_arg_array, $s_feldname);
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):               ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):         ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'rechnungsform'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_rechnungsform($a_return=0, $a_arg_array=array(), $s_feldname='rechnungsform')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

	// Auswahl-Array erstellen
	$arg_array=array('a_name'=>$s_feldname, 'a_select'=>$a_arg_array['a_select'], 'a_change'=>$a_arg_array['a_change']);
	$a_array=select_pdfconfig(2);
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):        ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):  ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'RK_Art'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_RK_Art($a_return=0, $a_arg_array=array(), $s_feldname='RK_Art')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

	// Ungültige Kategorie-IDs
	$invalid_ids='0';
	if((isset($spa) && !$spa) || (isset($pex) && !$pex))
	{
		if(!isset($art) || ($art != 1981))
			$invalid_ids .= ',1981';
		if(!isset($art) || ($art != 1982))
			$invalid_ids .= ',1982';
	}

	// Select-Argumente vorbereiten
	$s_name  = "SELECT wertetabid, tabwert FROM wertetab";
	$s_name .= " WHERE id_feld=1884 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id IN(0,".$Mandanten_ID.")";
	$s_name .= " AND wertetabid NOT IN (".$invalid_ids.")";
	$s_name .= " ORDER BY position,tabwert";
	$s_func='';
	$s_wert='wertetabid';
	$s_feld='tabwert';

	// Auswahl-Array erstellen
	$arg_array=argArray($s_name,$s_func,$s_feld,$s_wert,$s_feldname,$a_arg_array);
	$a_array=select_array($arg_array);
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):              ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):        ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'RK_Kategorie'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_RK_Kategorie($a_return=0, $a_arg_array=array(), $s_feldname='RK_Kategorie')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

	// Ungültige Kategorie-IDs
	$invalid_ids='1951,1952,1953,1954,1966,1980,2213,2214';	// 1951=Motorrad/Roller, 1952=Moped/Mofa, 1953=Fahrrad, 1954=Mitfahrer Motorrad/Roller, 1966=Kein Fahrzeug/Freier Betrag, 1980=Tagesangaben, 2213=Personenkraftwagen + 2214=Mitfahrer Personenkraftwagen
	if($uepausch_set)
		$invalid_ids .= ',2215,2216';						// 2215=Hotel (inkl. Verpflegung) + 2216=Hotel (ohne Verpflegung)
//$invalid_ids='0';

	// Select-Argumente vorbereiten und Auswahl-Array erstellen
	if(!fxIsArray($GLOBALS['RK_Kategorie_array']) || !fxIsArray($GLOBALS['MwSt_Satz_array']))
	{
		$GLOBALS['RK_Kategorie_array']=array();
		$GLOBALS['MwSt_Satz_array']=array();

		$sql  = "SELECT";
		$sql .= " mandanten_id, wertetabid, position, tabwert FROM wertetab ";
		$sql .= "WHERE";
		$sql .= " id_feld=1291 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id IN(0,".$Mandanten_ID.") AND wertetabid NOT IN (".$invalid_ids.") ";
		$sql .= "ORDER BY";
		$sql .= " position,tabwert";
		$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
		if(fxIsArray($tmp))
		{
			$arr=array();
			foreach($tmp as $t)
			{
				$id=$t['wertetabid'];
				if(!isset($arr[$id]) || $t['mandanten_id'])
					$arr[$id]=$t['tabwert'];
			}
			// Mehrwertsteuersätze der Einzelbeläge ermitteln
			if(fxIsArray($arr))
			{
				$vat_rate_normal=get_double($GLOBALS['fxpglobals']['settings']['mwst_satz']);
				$msa=array();
				foreach($arr as $id => $val)
				{
					$mp=strpos($val,'^');
					if($mp !== false)
					{
						$GLOBALS['MwSt_Satz_array'][$id]=getVATRate(substr($val,$mp+1));
						$arr[$id]=substr($val,0,$mp);
					}
					else
						$GLOBALS['MwSt_Satz_array'][$id]=$vat_rate_normal;
					$msa[(string)$GLOBALS['MwSt_Satz_array'][$id]]=true;
				}
				// Einzelbelege-Array nach Mehrwertsteuersätzen sortieren
				$mc=0;
				krsort($msa);
//fxDebug($msa,'$msa');
				foreach($msa as $md => $dmy)
				{
					if(sizeof($GLOBALS['RK_Kategorie_array']))
						$GLOBALS['RK_Kategorie_array'][--$mc]='<d>';
					$GLOBALS['RK_Kategorie_array'][--$mc]='&nbsp;<b>'.get_entry($md,'dezimal').'&nbsp;%</b><bb><bg#f0f0f0><d>';
					foreach($GLOBALS['MwSt_Satz_array'] as $id => $ms)
					{
						if($ms == $md)
							$GLOBALS['RK_Kategorie_array'][$id]='&nbsp;'.$arr[$id];
					}
				}
			}
		}
	}
//fxDebug($GLOBALS['RK_Kategorie_array'],'RK_Kategorie_array');
//fxDebug($GLOBALS['MwSt_Satz_array'],'MwSt_Satz_array');

	// MwSt-Satz je Einzelbelegsart schreiben für JavaScript Funktion "updateVoucherVAT"
	$ms='';
	if(fxIsArray($GLOBALS['MwSt_Satz_array']))
	{
		foreach($GLOBALS['MwSt_Satz_array'] as $id => $val)
			$ms .= '|'.$id.'^'.$val;
	}
	echo('<div id="RK_Kategorie_MwSt_Satz" style="display:none;">'.$ms.'</div>');
	if($a_return == 1)
		return $GLOBALS['RK_Kategorie_array'];

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $GLOBALS['RK_Kategorie_array'], $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):           ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):     ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'MwSt_Satz'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_MwSt_Satz($a_return=0, $a_arg_array=array(), $s_feldname='MwSt_Satz')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

	// Auswahl-Array erstellen
	$arg_array=array('a_name'=>$s_feldname, 'a_select'=>$a_arg_array['a_select'], 'a_change'=>$a_arg_array['a_change']);

	$a_array['0']=get_entry(0, 'dezimal').' %';
	$mnc='0';

	if(!$onlyzero)
	{
		$vat_rate_standard=get_double($GLOBALS['fxpglobals']['settings']['mwst_satz']);
		$vak=(string)$vat_rate_standard;
		$mnc .= ', '.$vak;
		$a_array[$vak]=get_entry($vat_rate_standard, 'dezimal').' %';

		$vat_rate_reduced=get_double($GLOBALS['fxpglobals']['settings']['mwst_satz_erm']);
		$vak=(string)$vat_rate_reduced;
		$mnc .= ', '.$vak;
		$a_array[$vak]=get_entry($vat_rate_reduced, 'dezimal').' %';

		if(fxIsArray($GLOBALS['MwSt_Satz_array']))
		{
			foreach($GLOBALS['MwSt_Satz_array'] as $id => $sval)
			{
				$vak=(string)$sval;
				if(!isset($a_array[$vak]))
				{
					$mnc .= ', '.$vak;
					$a_array[$vak]=get_entry($sval, 'dezimal').' %';
				}
			}
		}

		// Alle bisher verwendeten Sätze auch auflisten?
		if(strtoupper(substr($GLOBALS['fxpglobals']['settings']['mwst_satz_alle'],0,1)) == 'J')
		{
			$mta=array('materialverbrauch', 'rechnungseingang', 'rechnungsposition', 'sachmittel', 'zeitdaten');
			foreach($mta as $tn)
			{
				$sql="SELECT DISTINCT(mwst_satz) FROM ".$tn." WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND mwst_satz IS NOT NULL AND mwst_satz NOT IN (".$mnc.")";
				$maa=db_values($sql,'*');
//fxDebug($maa,$sql);
				if(fxIsArray($maa))
				{
					foreach($maa as $ma)
					{
						$vak=(string)$ma['mwst_satz'];
						$mnc .= ', '.$vak;
						$a_array[$vak]=get_entry($ma['mwst_satz'], 'dezimal').' %';
					}
				}
			}
		}
	}

	if(fxIsArray($GLOBALS['maskenfeldwerte']) && isset($GLOBALS['maskenfeldwerte'][strtolower($s_feldname)]))
	{
		$vak=(string)get_double($GLOBALS['maskenfeldwerte'][strtolower($s_feldname)]);
		if(!isset($a_array[$vak]))
			$a_array[$vak]=get_entry($vak, 'dezimal').' %';
	}
	if(fxIsArray($GLOBALS['vat_used']))
	{
		foreach($GLOBALS['vat_used'] as $sval => $dmy)
		{
			$vak=(string)$sval;
			if(!isset($a_array[$vak]))
				$a_array[$vak]=get_entry($sval, 'dezimal').' %';
		}
	}
	krsort($a_array);
	foreach($a_array as $sval => $dval)
	{
		if((double)$sval < 10.00)
			$a_array[$sval]='&nbsp;&nbsp;'.$dval;
	}
//fxDebug($arg_array,'$arg_array: $s_feldname='.$s_feldname); fxDebug($a_array,'$a_array: $s_feldname='.$s_feldname);

	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):            ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):      ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'buendelpos'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_buendelpos($a_return=0, $a_arg_array=array(), $s_feldname='buendelpos')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];
	if(!$bmax)
		$bmax=12;

	// Auswahl-Array erstellen
	$arg_array=array('a_name'=>$s_feldname, 'a_select'=>$a_arg_array['a_select'], 'a_change'=>$a_arg_array['a_change']);

	$a_array=array(0 => '00 ('.meldung(388, true).')');	// Ohne
	for($bc=1; $bc<=$bmax; $bc++)
		$a_array[$bc]=substr('00'.$bc, -2);

	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):              ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):        ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'rechnungstyp'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_rechnungstyp($a_return=0, $a_arg_array=array(), $s_feldname='rechnungstyp')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];
	if(!$Kunde)
		$Kunde=0;

	// Auswahl-Array erstellen
	$arg_array=array('a_name'=>$s_feldname, 'a_select'=>$a_arg_array['a_select'], 'a_change'=>$a_arg_array['a_change']);

	if($GLOBALS['fxpglobals']['tr'] == 117)	// Incoming Invoices
	{
		$a_array=array();
		$sql="SELECT wertetabid,tabwert FROM wertetab WHERE id_feld=2018 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id=0 ".db_orderby('position','tabwert');
		$tar=db_values($sql,'*');
		foreach($tar as $t)
			$a_array[$t['wertetabid']]=$t['tabwert'];
	}
	else
	{
		// Personen-ID des Mandanten ermitteln
		$sql="SELECT personen_id FROM personen WHERE personentyp=274 AND mandanten_id=".$Mandanten_ID;
		$Mand=(int)db_value($sql);

		$a_array=array(0 => '['.meldung(872, true).']');	// Rechnung
		$p_array=array($Kunde, $Mand);
		foreach($p_array as $pid)
		{
			$pid =(int)$pid;
			$sql ="SELECT kommunikation_id, kommunikation_txt FROM kommunikation";
			$sql .= " WHERE personen_id=".$pid." AND mandanten_id=".$Mandanten_ID." AND kommunikationstyp=2121";	// 2121=Rechnungstyp
			$sql .= " ORDER BY kommunikation_txt";
			$tmp =db_values($sql, '*');
	//fxDebug($tmp, $sql);
			if(is_array($tmp) && sizeof($tmp))
			{
				foreach($tmp as $t)
					$a_array[(int)$t['kommunikation_id']]=trim($t['kommunikation_txt']);
				break;
			}
		}
	}

	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):            ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):      ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'person_anz'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_person_anz($a_return=0, $a_arg_array=array(), $s_feldname='person_anz')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Auswahl-Array erstellen
	$GLOBALS[$s_feldname.'_ds']=0;
	$arg_array=array('a_name'=>$s_feldname, 'a_select'=>$a_arg_array['a_select'], 'a_change'=>$a_arg_array['a_change']);

	$a_array=array();
	if(isset($GLOBALS[$s_feldname.'_array']))
		$a_array=$GLOBALS[$s_feldname.'_array'];
	if(!is_array($a_array) || !sizeof($a_array))
	{
		$column='';
		if(isset($GLOBALS[$s_feldname.'_column']))
			$column=$GLOBALS[$s_feldname.'_column'];
		if(strlen($column))
		{
			if(trim($column) === (string)((int)$column))
				$column=get_text((int)$column, '20');
		}
		if(!strlen($column))
			$column='???';
		// 2815: nein + 2816: ja, in eigener Spalte + 2817: ja, integrieren in Spalte [$var1]
		$a_array=array(2815=>meldung(2815, false), 2816=>meldung(2816, false), 2817=>meldung(2817, true, $column));
	}

	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):             ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):       ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'vorgang_anz'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_vorgang_anz($a_return=0, $a_arg_array=array(), $s_feldname='vorgang_anz')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return auswahl_person_anz($a_return, $a_arg_array, $s_feldname);
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):           ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):     ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'kunde_anz'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_kunde_anz($a_return=0, $a_arg_array=array(), $s_feldname='kunde_anz')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return auswahl_person_anz($a_return, $a_arg_array, $s_feldname);
}

/**
 * Ansprechpartner für Förderprojekte
 *
 * @param integer $a_return    - Optional parameter (default = 0):           ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):     ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'fpp_anspr'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_fpp_anspr($a_return=0, $a_arg_array=array(), $s_feldname='fpp_anspr')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$GLOBALS[$s_feldname.'_arg']=array('modus'=>2, 'ptyp_arr'=>array('ptyp_s'=>1, 'mand'=>$GLOBALS['fxpglobals']['client']));
	return auswahl_Personen_ID($a_return, $a_arg_array, $s_feldname);
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):           ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):     ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'pdfs_form'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_pdfs_form($a_return=0, $a_arg_array=array(), $s_feldname='pdfs_form')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);

	$GLOBALS[$s_feldname.'_ds']=0;
	$arg_array=array('a_name'=>$s_feldname, 'a_select'=>$a_arg_array['a_select'], 'a_change'=>$a_arg_array['a_change']);
	$a_array=select_pdfconfig(1);
	if(fxIsArray($a_array) && (sizeof($a_array) > 1))
	{
		foreach($a_array as $av => $at)
		{
			if(strpos($av,'/0/') !== false)
				unset($a_array[$av]);
		}
	}
	if($a_return == 1)
		return $a_array;

	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):             ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):       ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'pdfs_format'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_pdfs_format($a_return=0, $a_arg_array=array(), $s_feldname='pdfs_format')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);

	$a_array=array('0x0'=>'['.meldung(629).': ('.$sside.' x '.$lside.' mm)]', '841x1189'=>'DIN A0 (841 x 1189 mm)', '594x841'=>'DIN A1 (594 x 841 mm)', '420x594'=>'DIN A2 (420 x 594 mm)', '297x420'=>'DIN A3 (297 x 420 mm)', '210x297'=>'DIN A4 (210 x 297 mm)');
	if($a_return == 1)
		return $a_array;

	$a_arg_array['a_name']=$s_feldname;
	$a_arg_array['a_defsel']=0;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):              ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):        ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'Kategorie_ID'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_Kategorie_ID($a_return=0, $a_arg_array=array(), $s_feldname='Kategorie_ID')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

	pk_getCategories();
//fxDebug($GLOBALS['pg'], 'pg');

	$s_name=array();
	if(fxIsArray($GLOBALS['pg']))
	{
		$i=0;
		foreach($GLOBALS['pg'] as $g)
		{
			if($i)
			{
				$s_name[$i]=array('id'=>-$g['id'], 'dval'=>'<d>');
				$i++;
			}

			$s_name[$i]=array('id'=>$g['id'], 'mid'=>$g['mid'], 'pos'=>$g['pos'], 'col'=>$g['col'], 'bg'=>$g['bg'], 'val'=>$g['val'], 'rval'=>$g['rval'], 'dval'=>$g['pos'].': <b>'.$g['rval'].'</b>');
			$i++;

			if(fxIsArray($g['cat']))
			{
				foreach($g['cat'] as $c)
				{
					$s_name[$i]=array('id'=>$c['id'], 'mid'=>$c['mid'], 'pos'=>$c['pos'], 'col'=>$c['col'], 'bg'=>$c['bg'], 'val'=>$c['val'], 'rval'=>$c['rval'], 'dval'=>'&nbsp;&nbsp;&nbsp;'.$g['pos'].'.'.$c['pos'].': '.$c['rval']);
					$i++;
				}
			}
		}
	}

	// Select-Argumente vorbereiten
	$s_wert='id';
	$s_feld='dval';

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$arg_array=argArray($s_name,$s_func,$s_feld,$s_wert,$s_feldname,$a_arg_array);
	$a_array=select_array($arg_array);
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param integer $a_return    - Optional parameter (default = 0):          ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):    ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'lagerort'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_lagerort($a_return=0, $a_arg_array=array(), $s_feldname='lagerort')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

	$s_name=array();
	$sql="SELECT lagerort FROM sachmittel WHERE lagerort IS NOT NULL AND mandanten_id=".$Mandanten_ID." ".db_orderby('lagerort');
	$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
	if(fxIsArray($tmp))
	{
		$oa=array();
		foreach($tmp as $t)
		{
			$ort=strtolower(trim($t['lagerort']));
			if(!isset($oa[$ort]))
				$oa[$ort]=trim($t['lagerort']);
		}
		if(sizeof($oa))
		{
			ksort($oa);
			$oc=0;
			foreach($oa as $ort => $text)
			{
				$s_name[$oc]=array('ort'=>$ort, 'text'=>$text);
				$oc++;
			}
		}
	}
//fxDebug($s_name,'$s_name: '.$s_feldname);

	// Select-Argumente vorbereiten
	$s_wert='ort';
	$s_feld='text';

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$arg_array=argArray($s_name,$s_func,$s_feld,$s_wert,$s_feldname,$a_arg_array);
	$a_array=select_array($arg_array);
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * AUSWAHLSFUNKTION-TEMPLATE (FELDNAME ersetzen mit richtigen Feldnamen)
 *
 * @param integer $a_return    - Optional parameter (default = 0):          ???PARAMETER???
 * @param array   $a_arg_array - Optional parameter (default = array()):    ???PARAMETER???
 * @param string  $s_feldname  - Optional parameter (default = 'FELDNAME'): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function auswahl_FELDNAME($a_return=0, $a_arg_array=array(), $s_feldname='FELDNAME')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Initialization values
	$arg=$GLOBALS[$s_feldname.'_arg'];
	if(fxIsArray($arg))
		extract($arg);
	if(!$Mandanten_ID)
		$Mandanten_ID=$GLOBALS['fxpglobals']['client'];

	// Select-Argumente vorbereiten
	$s_name="";	// SELECT-SQL, kann auch bereits fertiges Array sein
	$s_func='';	// Funktion zum Ändern der Selectwerte
	$s_wert='';	// SELECT-WERT		Array-Aufbau:
	$s_feld='';	// SELECT-FELD		$a_array[s_wert]="s_feld";

	// Auswahl-Array erstellen
	$arg_array=argArray($s_name,$s_func,$s_feld,$s_wert,$s_feldname,$a_arg_array);
	$a_array=select_array($arg_array);
	if($a_return == 1)
		return $a_array;

	// Auswahl-Array in Auswahl-Feld umwandeln und zurückliefern
	$a_arg_array['a_name']=$s_feldname;
	$a_html=convert_array($a_return, $a_array, $a_arg_array);
	if($a_return)
		return '~'.$a_html;
	else
		return $a_html;
}

/**
 * ???FUNCTION???
 *
 * @param various $s_name     - Mandatory parameter: ???PARAMETER???
 * @param various $s_func     - Mandatory parameter: ???PARAMETER???
 * @param various $s_feld     - Mandatory parameter: ???PARAMETER???
 * @param various $s_wert     - Mandatory parameter: ???PARAMETER???
 * @param various $s_feldname - Mandatory parameter: ???PARAMETER???
 * @param various $aa         - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function argArray($s_name,$s_func,$s_feld,$s_wert,$s_feldname,$aa)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!fxIsArray($aa))
		$aa=array('a_select'=>"",'a_change'=>false);

	$a=array(
		's_name'	=> $s_name,
		's_func'	=> $s_func,
		's_feld'	=> $s_feld,
		's_wert'	=> $s_wert,
		'a_name'	=> $s_feldname,
		'a_select'	=> $aa['a_select'],
		'a_change'	=> $aa['a_change']
	);

	return $a;
}
?>