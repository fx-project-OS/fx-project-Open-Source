<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : tools4.inc                                                   //
// Version     : 21.2                                                         //
// Begin       : 2020-08-20                                                   //
// Last Change : 2021-10-01                                                   //
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
 * Function collection
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$GLOBALS['__loaded_'.basename(__FILE__)]=true;

$GLOBALS['_us_replace']=array
(
	'%u02F5'=>'"', '%u02F6'=>'"',
	'%u03B1'=>"[Alpha]", '%u03B2'=>"[Beta]", '%u03C0'=>"[Pi]",
	'%u2013'=>"-", '%u2014'=>"-", '%u2018'=>"´", '%u2019'=>"`", '%u201C'=>'"', '%u201D'=>'"', '%u2026'=>"...", '%u203C'=>"!!",
	'%u2122'=>"(tm)", '%u2126'=>"[Omega]", '%u2192'=>"->",
	'%u2211'=>"Sum", '%u221E'=>"oo", '%u2237'=>"::", '%u2254'=>":=", '%u2260'=>"<>", '%u2264'=>"<=", '%u2265'=>">=",
	'%uF04A'=>":)"
);

$GLOBALS['_cs_replace']=$GLOBALS['_us_replace'];
$GLOBALS['_cs_replace']["'"]="''";

$GLOBALS['_ds_replace']=$GLOBALS['_us_replace'];
$GLOBALS['_ds_replace'][chr(0x5C).chr(0x22)]='"';

$GLOBALS['_ds_replace'][$nl.'<BR>'.$nl]=$nl;
$GLOBALS['_ds_replace']['<BR>'.$nl]=$nl;
$GLOBALS['_ds_replace']['<BR>']=$nl;
$GLOBALS['_ds_replace'][$nl.'<BR />'.$nl]=$nl;
$GLOBALS['_ds_replace']['<BR />'.$nl]=$nl;
$GLOBALS['_ds_replace']['<BR />']=$nl;

$GLOBALS['_ds_replace'][$nl.'<br>'.$nl]=$nl;
$GLOBALS['_ds_replace']['<br>'.$nl]=$nl;
$GLOBALS['_ds_replace']['<br>']=$nl;
$GLOBALS['_ds_replace'][$nl.'<br />'.$nl]=$nl;
$GLOBALS['_ds_replace']['<br />'.$nl]=$nl;
$GLOBALS['_ds_replace']['<br />']=$nl;

/**
 * create_varscreate_arrays
 * sichert den übergebenen Variablenwert unter dem übergebenen. Dadurch geht die Variable beim nächsten Aufruf nicht verloren.
 *
 * @param various $value - Mandatory parameter:                ???PARAMETER???
 * @param string  $name  - Optional parameter (default = ''):  ???PARAMETER???
 */
function protectVar($value, $name='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!fxIsArray($GLOBALS['fxptdata']['backup']))
		$GLOBALS['fxptdata']['backup']=array();
	if(!fxIsArray($GLOBALS['fxptdata']['backup']))
		$GLOBALS['fxptdata']['backup']=array();

	if(strlen($name))
		$GLOBALS['fxptdata']['backup'][$name]=$value;
	else
		$GLOBALS['fxptdata']['backup'][$value]=$GLOBALS[$value];
}

/**
 * convert_string
 * Diese Funktion escaped die Hochkommas eines Datenbanktextes.
 * -----------------------------------------------------------------------------
 * Zu übergebende Variablen:
 * °°°°°°°°°°°°°°°°°°°°°°°°°
 * $cs_string:		String, welcher umgewandelt werden soll
 * $cs_direction:	'todb' 		-> Umwandeln ins DB-Format
 * 'fromdb'	-> Unwandeln aus DB <STANDARD>
 * -----------------------------------------------------------------------------
 * Autor: Walter_T
 *
 * @param various $cs_string       - Mandatory parameter:                      ???PARAMETER???
 * @param string  $cs_direction    - Optional parameter (default = 'fromdb'):  ???PARAMETER???
 * @param boolean $replace_unicode - Optional parameter (default = true):      ???PARAMETER???
 *
 * @return ???RETURN???
 */
function convert_string($cs_string, $cs_direction='fromdb', $replace_unicode=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(is_array($cs_string) || !strlen($cs_string))
		return '';

	// DB->fxp
	if($cs_direction == 'fromdb')
	{
		if($replace_unicode)
		{
			$ra=replaceUnicode($cs_string, 'u');
			$cs_string=$ra['result'];
		}
		return $cs_string;
	}

	// fxp->DB
	$cs_string=strtr($cs_string, $GLOBALS['_cs_replace']);
	if($replace_unicode)
	{
		$ra=replaceUnicode($cs_string, '');
		$cs_string=$ra['result'];
	}

	return $cs_string;
}

/**
 * ???FUNCTION???
 *
 * @param various $str     - Mandatory parameter:                 ???PARAMETER???
 * @param string  $replace - Optional parameter (default = 'd'):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function replaceUnicode($str, $replace='d')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$found=0;
	if(strlen($str))
	{
		if(strlen($replace))
			$str=strtr($str,$GLOBALS['_'.$replace.'s_replace']);

		$fp=strpos($str,'%u');
		while($fp !== false)
		{
			$sp=$fp;
			$ucode='';
			for($cp=2; $cp<6; $cp++)
			{
				$c=substr($str,$fp+$cp,1);
				$o=ord($c);
				if(($o < 48) || (($o > 57) && ($o < 65)) || ($o > 70))
				{
					$ucode='';
					break;
				}
				else
					$ucode .= $c;
			}
			if(strlen($ucode) == 4)
			{
				$str=str_replace('%u'.$ucode, '?', $str);
				$found++;
			}
			else
				$sp += 2;

			$fp=strpos($str,'%u',$sp);
		}
	}

	return array('result'=>$str, 'found'=>$found);
}

/**
 * ???FUNCTION???
 *
 * @param various $val - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dbValue2HTML($val)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$html='';
	if(strlen($val) && ($val != "NULL"))
	{
		$html=$val;
		if(substr($html,0,1) == "'")
			$html=substr($html,1);
		if(substr($html,-1) == "'")
			$html=substr($html,0,-1);
	}

	return $html;
}

/**
 * ???FUNCTION???
 *
 * @param various $str - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function quot2apos($str)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(isset($GLOBALS['noquot']))
		return $str;

	$a=array('&quot;'=>'"', '&apos;'=>"'");
	return strtr($str,$a);
}

/**
 * ???FUNCTION???
 *
 * @param various $str - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function apos2quot($str)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(isset($GLOBALS['noquot']))
		return $str;

	$a=array('"'=>'&quot;', "'"=>'&apos;');
	return strtr($str,$a);
}

/**
 * test_bit
 * Funktion zum Testen, ob ein Bit einer Zahl gesetzt ist oder nicht
 * Vergleichswert ermitteln durch Verschieben von 1 um $Bit-Stellen nach links
 * z.B. $Bit=3 -> Start: 00000001=1 -> nach verschieben 00001000=8
 * dann log. UND-Vergleich von $Zahl mit Vergleichswert
 * z.B. $Zahl		(58) 	00111010=58
 * 1 << $Bit	(3)		00001000= 8
 * --------------------------------------
 * return	    		00001000= 8, d.h. > 0, Bit 3 ist gesetzt
 *
 * @param various $Zahl - Mandatory parameter: ???PARAMETER???
 * @param various $Bit  - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function test_bit($Zahl, $Bit)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return ($Zahl & get_bit($Bit));
}

/**
 * ???FUNCTION???
 *
 * @param various $Bit - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_bit($Bit)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return (1 << $Bit);	// <<="logical shift left"
}

/**
 * ermittelt, ob eine Task bei einem bestimmten Mandanten aktiv ist, d.h. hier:
 * 1. sie muss als Sonderprogramm aktiviert sein
 * 2. sie muss mindestens einmal pro Woche laufen
 * @param int $prog_id die Programm-ID
 * @param int $mandanten_id die Mandaten-ID
 * @return boolean true, wenn die Task nach obiger Definition aktiv ist, false sonst
 */
function is_task_active($prog_id, $mandanten_id)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sql  = "SELECT s.prog_id";
	$sql .= " FROM scheduler_settings s";
	$sql .= " WHERE s.prog_id=".$prog_id." AND s.mandanten_id=".$mandanten_id." AND s.soll_beg_zeit<s.soll_end_zeit";
	$sql .= " AND (s.montag=1 OR s.dienstag=1 OR s.mittwoch=1 OR s.donnerstag=1 OR s.freitag=1 OR s.samstag=1 OR s.sonntag=1)";

	$result=(int)db_value($sql);

	if($result)
		return true;
	return false;
}

/**
 * send_mail
 * Diese Funktion schickt eine E-Mail an einen oder mehrere Empfänger
 * -----------------------------------------------------------------------------
 * Zu übergebende Variablen:
 * °°°°°°°°°°°°°°°°°°°°°°°°°
 * $mail_recipient:	Personen-ID des Empfängers (Array, falls mehrere Empfänger)
 * $mail_subject:	Betreff
 * $mail_message:	Inhalt der email als HTML-Text
 * $mail_sender:	Personen-ID des Absenders (Standard: eingeloggte Person)
 * $mail_priority:	Wichtigkeit [optional] (Standard: '0'=normal)
 * -----------------------------------------------------------------------------
 * Autor: Walter_T
 *
 * @param various $mail_recipient   - Mandatory parameter:                   ???PARAMETER???
 * @param various $mail_subject     - Mandatory parameter:                   ???PARAMETER???
 * @param various $mail_message     - Mandatory parameter:                   ???PARAMETER???
 * @param integer $mail_sender      - Optional parameter (default = 0):      ???PARAMETER???
 * @param integer $mail_priority    - Optional parameter (default = 0):      ???PARAMETER???
 * @param boolean $mail_attachments - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $mail_return_html - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $force_send       - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $mail_debug       - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function send_mail($mail_recipient, $mail_subject, $mail_message, $mail_sender=0, $mail_priority=0, $mail_attachments=false, $mail_return_html=false, $force_send=false, $mail_debug=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

//$mail_debug=true;
	if($mail_debug)
		$GLOBALS['sm_debug']=true;

	// Mandanten-Nr.
	if($GLOBALS['_mail_mandantenid'])
		$mandid=(int)$GLOBALS['_mail_mandantenid'];
	else
		$mandid=$GLOBALS['fxpglobals']['client'];
	if(!$mandid)
		$mandid=1;

	// Konfigurationsvariablen leeren
	$cfg_background	= '';
	$cfg_logo		= '';
	$cfg_lmargin	= '';
	$cfg_rmargin	= '';
	$cfg_head		= '';
	$cfg_foot		= '';

	$system_name	= 'fx-project';
	$system_email	= 'fxp@fx-project.org';

	// Globaler Empfänger
	$mid_force=array();
	if($force_send)
		$dpe='0';
	else
	{
		$dpe=(string)trim($GLOBALS['fxpglobals']['dbparam']['email']);
		if(!strlen($dpe))
			$dpe='0';
	}
	$dpa=explode('|', $dpe);
	$dpe=$dpa[0];
	if(sizeof($dpa) > 1)
	{
		foreach($dpa as $dc => $mid)
		{
			if($dc)
				$mid_force[(int)$mid]=true;
		}
	}

	$mail_date=date('r');

	// Empfänger (recipients) ermitteln
	$recipient_data=array();
	$use_dms=true;
	if(!tr_program(110))
		$use_dms=false;
	if($mail_sender === 'fxp')
	{
		$use_dms=false;
		if($mail_priority === 'system')
			$recipient_data[1]=array('id'=>1, 'name2'=>'Administrator', 'email'=>$mail_recipient, 'mails'=>array($mail_recipient=>true));
		else
			$recipient_data[1]=array('id'=>1, 'name2'=>$system_name, 'email'=>$system_email, 'mails'=>array($system_email=>true));
		$mail_recipient=1;
	}
	else
	{
		// ... als Array
		if(fxIsArray($mail_recipient))
		{
			foreach($mail_recipient as $mrid)
				$recipient_data[(int)$mrid]='';
		}
		// ... als String
		else
		{
			$mre=explode(',', $mail_recipient);
			$nc=-1;
			foreach($mre as $mrid)
			{
				$mrid=trim($mrid);
				$ap=strpos($mrid,'@');
				if($ap)
					$recipient_data[$nc]=array('id'=>$nc, 'name2'=>'', 'email'=>$mrid, 'mails'=>array($mrid=>true));
				else
					$recipient_data[(int)$mrid]='';
				$nc--;
			}
		}
		if(fxIsArray($recipient_data))
		{
			foreach($recipient_data as $rid => $dmy)
			{
				if(!fxIsArray($dmy))
				{
					$mi=get_email_adress($rid,true);
					if(!fxIsArray($mi) || !fxIsArray($mi[$rid]) || !fxIsArray($mi[$rid]['mails']))
						unset($recipient_data[$rid]);
					else
						$recipient_data[$rid]=$mi[$rid];
				}
			}
		}
	}
//fxDebug($recipient_data, '$recipient_data');
	if(!fxIsArray($recipient_data))
		return true;

	// Absender (sender) ermitteln
	if(($mail_sender === 'fxp') || ($mail_priority === 'system'))
	{
		$use_dms=false;
		$mail_sender=1;
		$sender_data=array('id'=>$mail_sender, 'name'=>$system_name, 'email'=>$system_email);
	}
	else
	{
		$sender_data=array();
		$mail_sender=trim($mail_sender);
		$ap=strpos($mail_sender,'@');
		if($ap)
			$sender_data=array('id'=>-1, 'name'=>'', 'email'=>$mail_sender);
		else
		{
			$mail_sender=(int)$mail_sender;
			if(!$mail_sender)
				$mail_sender=$GLOBALS['fxpglobals']['person'];
			// Benutzer-Email-Adressen
			$sql  = "SELECT p.personen_id, p.pname, p.vorname, b.e_mail_adresse FROM personen p, benutzer b";
			$sql .= " WHERE b.personen_id=p.personen_id AND p.personen_id=".$mail_sender." AND p.mandanten_id=".$mandid." AND b.mandanten_id=".$mandid;
			$tmp=db_values($sql);
			if(!fxIsArray($tmp))
			{
				// Personen-Standard-Email-Adressen
				$sql  = "SELECT p.personen_id, p.pname, p.vorname, k.kommunikation_txt AS e_mail_adresse FROM personen p, kommunikation k";
				$sql .= " WHERE k.personen_id=p.personen_id AND p.personen_id=".$mail_sender." AND p.mandanten_id=".$mandid." AND k.kommunikationstyp=2366 AND k.mandanten_id=".$mandid;
				$tmp=db_values($sql);
			}
			if(!fxIsArray($tmp))
			{
				// Admin-Standard-Email-Adressen
				$sql  = "SELECT p.personen_id, p.pname, p.vorname, b.e_mail_adresse FROM personen p, benutzer b";
				$sql .= " WHERE b.personen_id=p.personen_id AND b.benutzer_id=1 AND p.mandanten_id=".$mandid." AND b.mandanten_id=".$mandid;
				$tmp=db_values($sql);
			}
			if(!fxIsArray($tmp))
			{
				// Admin1-Standard-Email-Adressen
				$sql  = "SELECT p.personen_id, p.pname, p.vorname, b.e_mail_adresse FROM personen p, benutzer b";
				$sql .= " WHERE b.personen_id=p.personen_id AND b.benutzer_id=1 AND b.mandanten_id=1";
				$tmp=db_values($sql);
			}
			if(fxIsArray($tmp))
			{
				$mail_sender=(int)$tmp['personen_id'];

				$nm=trim($tmp['pname']);
				$vn=trim($tmp['vorname']);
				if(strlen($vn))
					$nm=$vn.' '.$nm;

				$sender_data=array('id'=>$mail_sender, 'name'=>$nm, 'email'=>$tmp['e_mail_adresse']);
			}
		}
	}
//fxDebug($sender_data,'$sender_data');
	if(!fxIsArray($sender_data))
		return true;

	if($mail_priority === 'system')
		$mail_priority=1;

	// Konfigurationsfile einladen und parsen
	$emp='';
	if(isset($GLOBALS['_preview_config']))
		$emailcfg=$GLOBALS['_preview_config'];
	else
	{
		$emc='cfg_email.cfg';
		$emp=$GLOBALS['datpath'].$mandid.'/'.$emc;
		$emailcfg=fxLoad($emp,1);
		if(!fxIsArray($emailcfg))
		{
			$emp=$GLOBALS['datpath'].'0/'.$emc;
			$emailcfg=fxLoad($emp,1);
		}
	}
	if(fxIsArray($emailcfg))
	{
//fxDebug($emailcfg,'$emailcfg: '.$emp, 0);
		$mode=0; // 0=nicht, 1=Background, 2=Logo, 3=LMargin, 4=RMargin, 5=Head, 6=Foot
		for($ec=0; $ec<sizeof($emailcfg); $ec++)
		{
			$ezeile=trim(strtolower($emailcfg[$ec]));
			if(substr($ezeile, 0, 2) == '//')
				continue;
			if($ezeile)
			{
				$cmp=substr($ezeile, 0, 5);
				switch($cmp)
				{
					case '#back':
						$mode=1;
						break;
					case '#logo':
						$mode=2;
						break;
					case '#l_ma':
						$mode=3;
						break;
					case '#r_ma':
						$mode=4;
						break;
					case '#head':
						$mode=5;
						break;
					case '#foot':
						$mode=6;
						break;
					default:
						if($mode == 1)						// Background
							$cfg_background=$ezeile;
						else if($mode == 2)					// Logo
							$cfg_logo=$ezeile;
						else if($mode == 3)					// Left margin
							$cfg_lmargin=$ezeile;
						else if($mode == 4)					// Right margin
							$cfg_rmargin=$ezeile;
						else if($mode == 5)					// Header
							$cfg_head .= $emailcfg[$ec];
						else if($mode == 6)					// Footer
							$cfg_foot .= $emailcfg[$ec];
				}
			}
		}
	}

	// Betreff
	$mail_subject=trim(strip_tags($mail_subject));

	if($GLOBALS['sm_debug'])
	{
		fxDebug($cfg_background,'$cfg_background', 0);
		fxDebug($cfg_logo,'$cfg_logo', 0);
		fxDebug($cfg_lmargin,'$cfg_lmargin', 0);
		fxDebug($cfg_rmargin,'$cfg_rmargin', 0);
		fxDebug($cfg_head,'$cfg_head', 0);
		fxDebug($cfg_foot,'$cfg_foot', 0);
		fxDebug($recipient_data,'$recipient_data', 0);
		fxDebug($sender_data,'$sender_data', 0);
	}

	$m481=meldung(481); // Mail-Infoblock,Absender,Empfänger,Verteiler,Betreff,Mandant,Datum
	$literale=explode(',', $m481);

	// PRE-Message
	$margin='';
	if(strlen($cfg_lmargin))
		$margin .= 'margin-left:'.$cfg_lmargin.'px;';
	if(strlen($cfg_rmargin))
		$margin .= 'margin-right:'.$cfg_rmargin.'px;';

	$message_pre .= '<html>'.$GLOBALS['nl'];
	$message_pre .= '<head>'.$GLOBALS['nl'];
	$message_pre .= '	<title>fx-project Mail</title>'.$GLOBALS['nl'];
	$message_pre .= '</head>'.$GLOBALS['nl'];

	// Mail-Hintergrund
	$bbg='';
	if(strlen($cfg_background))
	{
		if($cfg_background == 'fx-project')
			$cfg_background='data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAEAAAAAABCAYAAABeIWt8AAAAQ0lEQVR4nO3DMRHAQAgAsPeLqK5IwAIWXgUOqIUOHZO7nBPP/jEiNjO3qra79967M6Oqqqqqqqqqqqqqqqqqqqp++AIMoc/TT0aXFgAAAABJRU5ErkJggg==';
		else
			$cfg_background=fxGetImagePath($cfg_background, $mandid);
		if(strlen($cfg_background))
			$bbg=' background="'.$cfg_background.'"';
	}
	$message_pre .= '<body style="background-color:#f0f0f0;font-family:font-family:verdana,arial,helvetica,sans-serif;font-size:12pt;'.$margin.'"'.$bbg.'>'.$GLOBALS['nl'];

	// POST-Message
	$text=$cfg_head.$mail_message.$cfg_foot;
	$message_pst  = $text.$GLOBALS['nl'];
	$message_pst .= '</body>'.$GLOBALS['nl'];
	$message_pst .= '</html>';

	// Einstellungen
	if($force_send || $mid_force[$mandid])	// Mandant darf keine eigene Einstellung verwenden
		$ets_email=0;
	else
		$ets_email=(int)substr($GLOBALS['fxpglobals']['settings']['email'], 0, 1);

	// Alle Empfänger durchgehen
	$send_error=false;
	foreach($recipient_data as $persid => $rd)
	{
		// ...An Hauptadresse senden
		$recipient_org=$rd['email'];
		$recipient='';

//echo('<b>$ets_email</b>='.$ets_email.'<br />');
		if($ets_email !== 1)
		{
			switch($ets_email)
			{
				case 0:	// Use ini file setting
					if(($dpe !== '0') && strlen($dpe))
						$recipient=$dpe;
				break;

				case 3:	// Alle E-Mails an den Administrator senden
					$recipient=db_value("SELECT e_mail_adresse FROM benutzer WHERE benutzer_id=1 AND mandanten_id=".$mandid);
				break;
			}
		}
		if(!$recipient || isset($GLOBALS['force_recipient']))
			$recipient=$recipient_org;
//echo('<b>$recipient_org</b>='.$recipient_org.' -- <b>$recipient</b>='.$recipient.'<br />');

		// Prüfen, ob gültige E-Mail-Adresse
		if(!$mail_return_html && (strtolower(substr($recipient, 0, 5)) == 'dummy'))
			continue;

		// Mail-Logo
		if(strlen($cfg_logo))
		{
			if($cfg_logo == 'fx-project')
				$cfg_logo=fxGetLogo('medium');
			else
				$cfg_logo=fxGetImagePath($cfg_logo, $mandid);
		}

		$message_head='';

		// ...Icon
		if(strlen($cfg_logo))
			$message_head .= '<img src="'.$cfg_logo.'" border="0" align="right" hspace="16" vspace="16"><br />'.$GLOBALS['nl'];

		// Mail-Infoblock
		$message_head .= '<table width="80%" border="0" cellpadding="2" cellspacing="1" bgcolor="#aaaaaa">'.$GLOBALS['nl'];
		$message_head .= '	<tr>'.$GLOBALS['nl'];
		$message_head .= '		<td bgcolor="#dddddd">&nbsp;<font color="#000000"><b>fx-project:</b>&nbsp;&nbsp;'.$literale[0].'</font>&nbsp;</td>'.$GLOBALS['nl'];
		$message_head .= '	</tr>'.$GLOBALS['nl'];

		$message_head .= '	<tr>'.$GLOBALS['nl'];
		$message_head .= '		<td bgcolor="f8f8f8">'.$GLOBALS['nl'];
		$message_head .= '			<table width="100%" border="0" cellpadding="2" cellspacing="0">'.$GLOBALS['nl'];
		$message_head .= '				<tr>'.$GLOBALS['nl'];
		// Absender
		$message_head .= '					<td valign="top" nowrap><font color="#666666">&nbsp;'.$literale[1].':&nbsp;</font></td>'.$GLOBALS['nl'];
		$message_head .= '					<td valign="top" nowrap>';
		if(strlen($sender_data['name']))
			$message_head .= $sender_data['name'].'<br /><font color="#aaaaaa">('.$sender_data['email'].')</font>';
		else
			$message_head .= $sender_data['email'];
		$message_head .= '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>'.$GLOBALS['nl'];
		// Empfänger
		$message_head .= '					<td valign="top" nowrap><font color="#666666">&nbsp;'.$literale[2].':&nbsp;</font></td>'.$GLOBALS['nl'];
		$message_head .= '					<td width="100%" valign="top" nowrap>';
		if(strlen($rd['name2']))
			$message_head .= $rd['name2'].'<br /><font color="#aaaaaa">('.$recipient_org.')</font>';
		else
			$message_head .= $recipient_org;
		$message_head .= '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>'.$GLOBALS['nl'];
		// Mandant, Datum
		if($mandid)
			$mib_mid=(int)$mandid;
		else
			$mib_mid='?';
		if(fxIsArray($GLOBALS['fxpglobals']['settings']) && isset($GLOBALS['fxpglobals']['settings']['formatdatum']))
			$mib_dtm=get_entry(fxNow(), 'datetime');
		else
			$mib_dtm=fx_date('Y-m-d, H:i', time());
		$message_head .= '					<td valign="top" nowrap><font color="#666666">&nbsp;'.$literale[5].':&nbsp;</font><br /><font color="#666666">&nbsp;'.$literale[6].':&nbsp;</font></td>'.$GLOBALS['nl'];
		$message_head .= '					<td valign="top" nowrap>'.$mib_mid.'&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<br />'.$mib_dtm.'&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>'.$GLOBALS['nl'];
		$message_head .= '				</tr>'.$GLOBALS['nl'];

/*
		// Verteiler
		$message_head .= '				<tr>'.$GLOBALS['nl'];
		if(sizeof($recipient_data) > 1)
		{
			$vcc='';
			foreach($recipient_data as $ccpersid => $ccrd)
			{
				if($vcc)
					$vcc .= '; ';
				if($ccpersid == $persid)
					$vcc .= '<b>'.$ccrd['name2'].'</b>';
				else
					$vcc .= $ccrd['name'];
				$vcc .= ' <i>('.$ccrd['email'].')</i>';
			}
			$message_head .= '					<td valign="top" nowrap><br /><font color="#666666">&nbsp;'.$literale[3].':&nbsp;</font></td>'.$GLOBALS['nl'];
			$message_head .= '					<td colspan="5"><br />'.$vcc.'</td>'.$GLOBALS['nl'];
			$message_head .= '				</tr>'.$GLOBALS['nl'];
		}
*/

		// Betreff
		$message_head .= '				<tr>'.$GLOBALS['nl'];
		$message_head .= '					<td valign="top" nowrap><br /><font color="#666666">&nbsp;'.$literale[4].':&nbsp;</font></td>'.$GLOBALS['nl'];
		$message_head .= '					<td colspan="5"><br />'.$mail_subject.'</td>'.$GLOBALS['nl'];
		$message_head .= '				</tr>'.$GLOBALS['nl'];

		$message_head .= '			</table>'.$GLOBALS['nl'];
		$message_head .= '		</td>'.$GLOBALS['nl'];
		$message_head .= '	</tr>'.$GLOBALS['nl'];
		$message_head .= '</table>'.$GLOBALS['nl'];

		$message_head .= '<br /><br />'.$GLOBALS['nl'];

		$mtr_table=array("\r\n"=>'', "\r"=>'', "\n"=>'', "\t"=>'');
		$message=strtr($message_pre.$message_head.$message_pst, $mtr_table);
//fxDebug($message, '$message');

		if($mail_return_html)
			return $message;
		if($GLOBALS['sm_debug'])
			echo('<hr style="border:0;border-top:1px solid #888888;" />Email from <b style="border:1px solid black;padding:2px 4px;border-radius:4px;color:#006b9f;">'.$sender_data['email'].'</b> to <b style="border:1px solid black;padding:2px 4px;border-radius:4px;color:#009f6b;">'.$recipient.'</b><br /><hr style="border:0;border-top:1px solid #cccccc;" />'.$message.'<hr style="border:0;border-top:1px solid #444444;" />'.$GLOBALS['nl']);
//return false;

		// Betreff erweitern mit Mandanten-ID
		if($mandid)
			$mail_subject_m=str_replace('&quot;', '"', $mail_subject).' ['.$mandid.']';
		else
			$mail_subject_m=str_replace('&quot;', '"', $mail_subject);

		// Send with SMTP or with PHP mail function?
		$smtp_host=trim($GLOBALS['fxpglobals']['dbparam']['smtp_host']);
		$mail_sendmethod=getSendingMethod($smtp_host);

		// Send with SMTP and external mailer?
		if($mail_sendmethod == 'smtp')
		{
			if(isset($pmail))
				unset($pmail);

			// Create new PHP Mailer instance
			$pmail=false;
			if(isset($GLOBALS['_ext_mailer']) && strlen($GLOBALS['_ext_mailer']))
			{
				if($GLOBALS['_ext_mailer'] == 'PHPMAILER')
					$pmail=new PHPMailer\PHPMailer\PHPMailer(TRUE);
			}
			if($pmail)
			{
				// Get SMTP parameters: port, helo, email, account + password
				$smtpArray=getSMTPArray();
				// Use SMTP
				$pmail->isSMTP();
				// Enable SMTP debugging: 0=off (for production use), 1=client messages, 2=client and server messages
				if($mail_debug)
				{
					$pmail->SMTPDebug=2;
					$pmail->Debugoutput='html';
				}
				// Set the SMTP parameters
				$pmail->Host=$smtp_host;
				$pmail->Port=$smtpArray['port'];
				if($smtpArray['port'] == 587)
					$pmail->SMTPSecure='tls';
				if(strlen($smtpArray['helo']))
					$pmail->Helo=$smtpArray['helo'];
				if(strlen($smtpArray['email']))
					$pmail->SMTPAuth=$smtpArray['email'];
				else
					$pmail->SMTPAuth=true;
				$pmail->Username=$smtpArray['account'];
				$pmail->Password=$smtpArray['password'];

				// Header-Infos
				// ...Sender
				if(substr($sender_data['email'],-4) == '.xyz')
					$sender_data['email']=$system_email;
				$pmail->setFrom($sender_data['email']);
				$pmail->addReplyTo($sender_data['email']);
				// ...Recipient
				$pmail->addAddress($recipient);
				// ...Date
				$pmail->MessageDate=$mail_date;
				// ...XMailer
				$pmv=trim($pmail->VERSION);
				if(!strlen($pmv))
					$pmv='6.1.8';
				$pmail->XMailer='fx-project (PHP V'.phpversion().', PHPMailer V'.$pmv.')';
				// ...Priority
				if($mail_priority)
					$pmail->Priority=1;	// null (default), 1=High, 3=Normal, 5=low
				// ...CharSet/Encoding
				$pmail->CharSet=FXP_CHARSET;
				if(FXP_CHARSET == 'UTF-8')
					$pmail->Encoding='base64';

				// Subject
				$pmail->Subject=$mail_subject_m;

				// HTML body: Convert referenced images to embedded and HTML into a basic plain-text alternative body
				$pmail->isHTML(true);
				$pmail->msgHTML($message);

				// Attachments
				if(fxIsArray($mail_attachments))
				{
//fxDebug($mail_attachments,'mail_attachments', 0);
					foreach($mail_attachments as $ma_filepath => $ma_array)
					{
						$ppos=strrpos($ma_filepath, '.');
						if($ppos)
						{
							$suffix=strtolower(substr($ma_filepath,$ppos+1));
							$ma_type=getMIMETypes($suffix);
							if(is_array($ma_array) && isset($ma_array['name']))
								$ma_name=$ma_array['name'];
							else
							{
								$ma_name=$ma_filepath;
								while(true)
								{
									$sp=strpos($ma_name,'/');
									if($sp === false)
										break;
									$ma_name=substr($ma_name,$sp+1);
								}
							}
//echo('<b>Attachment:</b> <b>File</b>=&quot;'.$ma_filepath.'&quot; -- <b>Name:</b> &quot;'.$ma_name.'&quot;, <b>Typ:</b> '.$suffix.' -&gt; '.$ma_type.'<br />');
							$pmail->addAttachment($ma_filepath, $ma_name, 'base64', $ma_type);
						}
					}
				}

				// Send email or show debug informations
				if($GLOBALS['sm_debug'])
				{
		            $pmail->preSend();
					$mmail=$pmail->getSentMIMEMessage();
					echo('<pre style="border:1px solid #444444;padding:12px;border-radius:8px;">'.fxHtmlEncode($mmail).'</pre>');
					echo('<div style="border:1px solid #444444;border-top-left-radius:8px;border-top-right-radius:8px;padding:8px;color:#ffffff;background:#006b9f;"><b>'.$mail_subject_m.'</b></div><div style="border:1px solid #444444;border-top:0;border-bottom-left-radius:8px;border-bottom-right-radius:8px;padding:8px;background:#ffffff;margin-bottom:6px;">'.$message.'</div>'.$GLOBALS['nl']);
					$send_error='Debug';
				}
				else
				{
					$write_dms=true;
					if(!tr_program(110))
						$write_dms=false;
					$fiud=false;
					$filename='';
					if(!$force_send && $use_dms && $mandid && $GLOBALS['fxpglobals']['user'])
					{
			            $pmail->preSend();
						$mmail=$pmail->getSentMIMEMessage();

						$mic_time=$GLOBALS['datetime'].'_'.substr(microtime(),2,4);
						$orgname=$persid.'_email'.$mic_time.'.eml';
						$filename=$GLOBALS['fxpglobals']['dbparam']['upload_dir'].$orgname;
						$se=fxSave($filename, $mmail);
						if(!$se)	// Save successful
							$fiud=true;
					}

					if($ets_email !== 1)
					{
						$send_mail=true;
						if(($ets_email == 0) && ($dpe === '-1'))
						{
							$send_mail=false;
							$write_dms=false;
						}

						// Mail soll geschickt werden und E-Mail-Scheduler inaktiv
						// => sofort senden
						if($force_send || ($send_mail && !is_task_active(FXP_SDL_EMAIL, $mandid)))
						{
							$send_ok=$pmail->send();
							if(!$send_ok)
								$send_error=$pmail->ErrorInfo;
						}
					}
					else
						$write_dms=false;
//fxDebug(array('$force_send'=>$force_send, '$fiud'=>$fiud, '$write_dms'=>$write_dms, '$use_dms'=>$use_dms, '$mandid'=>$mandid, '$persid'=>$persid, '$GLOBALS(fxpglobals)(user)'=>$GLOBALS['fxpglobals']['user'], '$filename'=>$filename, '$send_mail'=>$send_mail, '$ets_email'=>$ets_email), 'send_mail', 0);

					// Mail ins DMS schreiben...
					if(!$force_send && $fiud && $write_dms)
					{
						// Dokument-ID
						$did=0;
						$seid='3/'.$persid.'.'.$did.'.0';	// E-Mails (elter_art=3), $persid=Person (elter)

						// Upload-Attribute
						$uarr=array(
							'error'		=> 0,
							'name'		=> $orgname,
							'size'		=> filesize($filename),
							'new_name'	=> $filename
						);

						// Dokument-Attribute
						$values=array(
							'beschreibung' => $mail_subject.' [FROM: '.$sender_data['name'].' ('.$sender_data['email'].'); TO: '.$rd['name'].' ('.$recipient.'); DATE: '.get_entry($GLOBALS['datetime'],'datum').']'
						);

						// Dokument in das DMS hinzufügen
						$doc=new fxpDoc();
						$em=explode('|', $doc->uploadDocument($seid, $uarr, $values));
//fxDebug($em,'$em', 0);
						$did=$doc->_docArray['did'];
//fxDebug($doc->_docArray, 'docArray', 0);
						if($send_ok && ($em[0] == 'OK') && ($did > 0))
							$err=$doc->writeLog($did, 64, 0.0, false);
					}
				}
			}
			else
				$send_error='No external mailer';
		}

		// Use PHP internal mailer
		else
		{
			// Headers
			$eh=array();
			// ...Sender
			if(substr($sender_data['email'],-4) == '.xyz')
				$sender_data['email']=$system_email;
			$eh['From']		= $sender_data['email'];
			$eh['Reply-To']	= $sender_data['email'];
			// ...XMailer
			$eh['X-Mailer']	= 'fx-project-mail (PHP V'.phpversion().')';
			// ...HTML emails
			$eh['MIME-Version']='1.0';
			$eh['Content-type']='text/html; charset='.FXP_CHARSET;

			// Send email
			$send_ok=@mail($recipient, $mail_subject_m, $message, $eh);
			if(!$send_ok)
				$send_error='Email sending error';
		}
	}

	return $send_error;
}

/**
 * send_dms_mail
 * Diese Funktion schickt eine E-Mail aus dem DMS an einen Empfänger
 * -----------------------------------------------------------------------------
 * Zu übergebende Variablen:
 * °°°°°°°°°°°°°°°°°°°°°°°°°
 * $dms_mail_filename: Dateiname der E-Mail im DMS
 * -----------------------------------------------------------------------------
 * Autor: Walter_T
 *
 * @param various $dms_mail_filename - Mandatory parameter:               ???PARAMETER???
 * @param integer $mail_debug        - Optional parameter (default = 0):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function send_dms_mail($dms_mail_filename, $mail_debug=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$mail_debug=(int)$mail_debug;

	// Read dms mail
	$dmail='';
	if(file_exists($dms_mail_filename))
	{
		$fp=@fopen($dms_mail_filename,'r');
		if($fp)
		{
			$dmail=fread($fp, filesize($dms_mail_filename));
			fclose($fp);
		}
	}

	if(strlen($dmail))
	{
		if($mail_debug > 0)
			echo('<hr /><b>MIMEMail</b> '.$dms_mail_filename.'<pre style="border:1px solid black;padding:12px;border-radius:8px;">'.fxHtmlEncode($dmail).'</pre>'.$GLOBALS['nl']);

		// Get header and body
		$mnl="\r\n";
		$dlp=(int)strpos($dmail, $mnl.$mnl, 1);
		if(!$dlp)
		{
			$mnl="\n";
			$dlp=(int)strpos($dmail, $mnl.$mnl, 1);
		}
//echo('$dlp='.$dlp.'<hr />');
		$mheader=trim(substr($dmail,0,$dlp));
		$mbody=trim(substr($dmail,$dlp));
//echo('<b>mheader</b><pre style="border:1px solid black;padding:12px;border-radius:8px;">'.fxHtmlEncode($mheader).'</pre>');
//echo('<b>mbody</b><pre style="border:1px solid black;padding:12px;border-radius:8px;">'.fxHtmlEncode($mbody).'</pre>');

		if(strlen($mheader) && strlen($mbody))
		{
			// Create new PHP Mailer instance
			if(isset($pmail))
				unset($pmail);
			$pmail=false;
			if(isset($GLOBALS['_ext_mailer']) && strlen($GLOBALS['_ext_mailer']))
			{
				if($GLOBALS['_ext_mailer'] == 'PHPMAILER')
					$pmail=new PHPMailer\PHPMailer\PHPMailer(TRUE);
			}
			if($pmail)
			{
				// Send with SMTP or with PHP mail function?
				$smtp_host=trim($GLOBALS['fxpglobals']['dbparam']['smtp_host']);
				$mail_sendmethod=getSendingMethod($smtp_host);

				// Get and set header infos
				$mheader_arr=array(
					'date'			=> array('key'=>"Date: ", 'val'=>""),
					'from'			=> array('key'=>"From: ", 'val'=>""),
					'return-path'	=> array('key'=>"Return-Path: ", 'val'=>""),
					'reply-to'		=> array('key'=>"Reply-To: ", 'val'=>""),
					'to'			=> array('key'=>"To: ", 'val'=>""),
					'subject'		=> array('key'=>"Subject: ", 'val'=>""),
					'message-id'	=> array('key'=>"Message-ID: ", 'val'=>""),
					'x-mailer'		=> array('key'=>"X-Mailer: ", 'val'=>""),
					'mime-version'	=> array('key'=>"MIME-Version: ", 'val'=>""),
					'content-type'	=> array('key'=>"Content-Type: ", 'val'=>"")
				);
				$boundary="";
				if($mail_debug > 1)
					echo('<hr /><b>MIMEMail: Old Header</b> '.$dms_mail_filename.'<pre style="border:1px solid black;padding:12px;border-radius:8px;">'.fxHtmlEncode($mheader).'</pre>'.$GLOBALS['nl']);
				$mha=explode("\n",$mheader);
				foreach($mha as $mhline)
				{
					$cp=strpos($mhline,':');
					$hi=strtolower(trim(substr($mhline,0,$cp)));
					$hv=trim(substr($mhline,$cp+1));
					if($mail_debug > 1)
						echo('$mhline=['.$mhline.']');
					if($cp !== false)
					{
						if($mail_debug > 1)
							echo(' -- $hi='.$hi.', $hv='.$hv.' -- ');

						if(isset($mheader_arr[$hi]))
							$mheader_arr[$hi]['val']=$hv;

						if($hi == 'from')
						{
							$pmail->setFrom($hv);
							if($mail_debug > 1)
								echo('<b>setFrom</b>(&quot;<b class=blue>'.$hv.'</b>&quot;)');
						}
						else if($hi == 'reply-to')
						{
							$pmail->addReplyTo($hv);
							if($mail_debug > 1)
								echo('<b>addReplyTo</b>(&quot;<b class=blue>'.$hv.'</b>&quot;)');
						}
						else if($hi == 'to')
						{
							$pmail->addAddress($hv, '');
							if($mail_debug > 1)
								echo('<b>addAddress</b>(&quot;<b class=blue>'.$hv.'</b>&quot;)');
						}
						else if($hi == 'date')
						{
							$pmail->MessageDate=$hv;
							if($mail_debug > 1)
								echo('<b>MessageDate</b>(&quot;<b class=blue>'.$hv.'</b>&quot;)');
						}
						else if($hi == 'x-mailer')
						{
							$pmail->XMailer=$hv;
							if($mail_debug > 1)
								echo('<b>XMailer</b>(&quot;<b class=blue>'.$hv.'</b>&quot;)');
						}
						else if($hi == 'priority')
						{
							$pmail->Priority=(int)$hv;
							if($mail_debug > 1)
								echo('<b>Priority</b>(<b class=blue>'.((int)$hv).'</b>)');
						}
						else if($hi == 'subject')
						{
							$pmail->Subject=$hv;
							if($mail_debug > 1)
								echo('<b>Subject</b>(&quot;<b class=blue>'.$hv.'</b>&quot;)');
						}
						else if($hi == 'content-type')
						{
							$csp=stripos($hv,'charset');
							if($csp !== false)
							{
								$csp=strpos($hv,'=',$csp+7);
								if($csp !== false)
								{
									$csp++;
									$cep=strpos($hv,';',$csp+1);
									if($cep !== false)
										$cs=trim(substr($hv,$csp,$cep-$csp));
									else
										$cs=trim(substr($hv,$csp));
									$pmail->CharSet=$cs;
									if($cs == 'UTF-8')
										$pmail->Encoding='base64';
								}
							}
						}
						else if($mail_debug > 1)
						{
							if(isset($mheader_arr[$hi]))
								echo('<i class=blue>(Parameter set!)</i>');
							else
								echo('<i class=red>(No action!)</i>');
						}
					}
					else if($mail_debug > 1)
						echo(' <i class=red>(No key!)</i>');

					// Check for boundary
					$cp=strpos($mhline, 'boundary');
					if($cp !== false)
					{
						$boundary=trim(strtr(substr($mhline,$cp+8), array('"'=>'', ';'=>'')));
						if(substr($boundary,0,1) == '=')
							$boundary=substr($boundary,1);
						$cp=strpos($boundary, ' ');
						if($cp !== false)
							$boundary=trim(substr($boundary,0,$cp));
						if($mail_debug > 1)
							echo(' -- <i class=blue>(Boundary set to: [<b>'.$boundary.'</b>])</i>');
					}
					if($mail_debug > 1)
						echo('<br />');
				}
//fxDebug($mheader_arr,'$mheader_arr: $boundary=['.$boundary.']');
				$mheader='';
				foreach($mheader_arr as $mhk => $mha)
				{
					if($mhk == 'content-type')
					{
						if(strlen($boundary) > 0)
							$mheader .= $mha['key'].'multipart/related; boundary="'.$boundary.'"; type="multipart/alternative"'.$mnl;
						else if(strlen($mha['val']) > 0)
							$mheader .= $mha['key'].$mha['val'].$mnl;
					}
					else if(strlen($mha['val']) > 0)
						$mheader .= $mha['key'].$mha['val'].$mnl;
				}
				$mheader=trim($mheader);
				if($mail_debug > 1)
					echo('<b>MIMEMail: New Header</b> '.$dms_mail_filename.'<pre style="border:1px solid black;padding:12px;border-radius:8px;">'.fxHtmlEncode($mheader).'</pre>'.$GLOBALS['nl']);

				// Send with SMTP
				if($mail_sendmethod == 'smtp')
				{
					// Get SMTP parameters: port, helo, email, account + password
					$smtpArray=getSMTPArray();
					// Use SMTP
					$pmail->isSMTP();
					// Enable SMTP debugging: 0=off (for production use), 1=client messages, 2=client and server messages
					if($mail_debug > 0)
					{
						$pmail->SMTPDebug=2;
						$pmail->Debugoutput='html';
					}
					// Set the SMTP parameters
					$pmail->Host=$smtp_host;
					$pmail->Port=$smtpArray['port'];
					if($smtpArray['port'] == 587)
						$pmail->SMTPSecure='tls';
					if(strlen($smtpArray['helo']))
						$pmail->Helo=$smtpArray['helo'];
					if(strlen($smtpArray['email']))
						$pmail->SMTPAuth=$smtpArray['email'];
					else
						$pmail->SMTPAuth=true;
					$pmail->Username=$smtpArray['account'];
					$pmail->Password=$smtpArray['password'];
					if($mail_debug > 255)
						echo('<b>SMTP:</b><hr />Host='.$smtp_host.'<br />Helo='.$smtpArray['helo'].'<br />SMTPAuth='.$smtpArray['email'].'<br />Username='.$smtpArray['account'].'<br />Password='.$smtpArray['password'].'<hr />');
				}

				$pmail->set('MIMEHeader', $mheader);
				$pmail->set('MIMEBody', $mbody);
				if($mail_debug > 127)
					fxDebug($pmail,'$pmail');

				if($mail_debug > 1023)
				{
					$pmail->preSend();
					file_put_contents('__MIMEMail_'.$GLOBALS['datetime'].'_'.$boundary.'.eml', $pmail->getSentMIMEMessage());
					echo('<b>New MIMEMail</b> '.$dms_mail_filename.'<pre style="border:1px solid black;padding:12px;border-radius:8px;">'.fxHtmlEncode($pmail->getSentMIMEMessage()).'</pre>'.$GLOBALS['nl']);
				}
				$send_ok=$pmail->postSend();
//$send_ok=false;
				if(!$send_ok)
					return $pmail->ErrorInfo;

				return false;
			}
		}
	}

	return true;
}

/**
 * send_password
 * Diese Funktion schickt in einer e-mail ein zufälliges Passwort an den Be-
 * nutzer und setzt seinen Status auf 1="aktiv"
 * -----------------------------------------------------------------------------
 * Zu übergebende Variablen:
 * °°°°°°°°°°°°°°°°°°°°°°°°°
 * $uid:	Benutzer_ID aus der Datenbank
 * -----------------------------------------------------------------------------
 * Beispiel:
 * °°°°°°°°°
 * send_password(9);
 * ->	sendet ein zufälliges Passwort an den Benutzer
 * -----------------------------------------------------------------------------
 * Autor: Walter_T
 *
 * @param various $uid - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function send_password($uid)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$uid=(int)$uid;

	// User info
	$sql="SELECT personen_id, name_benutzer, status_benutzer, e_mail_adresse FROM benutzer WHERE benutzer_id=".$uid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$uda=db_values($sql);
//fxDebug($uda,$sql, 0);
	if(!fxIsArray($uda) || !strlen($uda['e_mail_adresse']))
		return 1167;	// The user "$var1" either doesn't have an Email address or it is not correct. The record was not changed!

	// Create and encode a new random 8 character password
	$npw=hexPassword(8);
	$epw=fxf_ecPasswd($npw, '', $GLOBALS['fxpglobals']['client'], $uid);

	// Set new password / activate user
	$sql="UPDATE benutzer SET status_benutzer=1, passwort='".$epw."' WHERE benutzer_id=".$uid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$err=db_x($sql);
	if($err)
		return 1166;	// User status and password for "$var1" could not be changed.

	// Send email
	if(check_mail(1738,'MA'))
	{
		if(!isset($GLOBALS['_ua']))
			$GLOBALS['_ua']=array();
		if(!isset($GLOBALS['_ma']))
			$GLOBALS['_ma']=array();

		$settings_backup=$GLOBALS['fxpglobals']['settings'];
		$lang_backup=$GLOBALS['fxpglobals']['lang'];

		// Get sender data
		$sender=$GLOBALS['fxpglobals']['person'];
		if(!isset($GLOBALS['_ua'][$sender]))
		{
			$pdata=get_email_adress($sender,true,true);
			if(fxIsArray($pdata) && fxIsArray($pdata[$sender]))
				$GLOBALS['_ua'][$sender]=$pdata[$sender];
			else
				$GLOBALS['_ua'][$sender]=false;
		}

		// Get recipient data
		$recipient=$uda['personen_id'];
		if(!isset($GLOBALS['_ua'][$recipient]))
		{
			$pdata=get_email_adress($recipient,true,true);
			if(fxIsArray($pdata) && fxIsArray($pdata[$recipient]))
				$GLOBALS['_ua'][$recipient]=$pdata[$recipient];
			else
				$GLOBALS['_ua'][$recipient]=false;
		}

		// Send email
		$GLOBALS['fxpglobals']['settings']=$GLOBALS['_ua'][$recipient]['settings'];
		$GLOBALS['fxpglobals']['lang']=$GLOBALS['_ua'][$recipient]['settings']['sprache'];
		$lang=$GLOBALS['fxpglobals']['lang'];

		// The administrator {admin} has generated a <b style="color:#009f6b;">new</b> password for your user account in <b>fx-project</b>.^<br />The lock on your user account in <b>fx-project</b> has been <b style="color:#009f6b;">lifted</b>.^<br /><br /><hr width="80%" align="left" size="1" /><br />Please use the following information to log in to <b>fx-project</b>:<br /><br /><br /><table cellpadding="1" cellspacing="1" bgcolor="#666666" style="border-radius:12px;box-shadow:8px 8px 8px #666666;"><tr><td bgcolor="#fafafa" style="border-radius:12px;"><table cellpadding="2" cellspacing="1" style="border-radius:12px;"><tr><td colspan="3" bgcolor="#ececec" style="border-top-left-radius:12px;border-top-right-radius:12px;">&nbsp;&nbsp;<b style="font-size:16pt;color:#666666;">User login</b></td></tr><tr><td colspan="3">&nbsp;</td></tr><tr><td align="right">&nbsp;&nbsp;<font style="color:#8f788f;">Client No.:</font>&nbsp;</td><td width="150" style="border:1px solid #b5cfdc;">&nbsp;<b>{client}</b>&nbsp;</td><td rowspan="3">&nbsp;&nbsp;</td></tr><tr><td align="right">&nbsp;&nbsp;<font style="color:#8f788f;">User Name:</font>&nbsp;</td><td style="border:1px solid #b5cfdc;">&nbsp;<b>{uname}</b>&nbsp;</td></tr><tr><td align="right">&nbsp;&nbsp;<font style="color:#8f788f;">Password:</font>&nbsp;</td><td style="border:1px solid #b5cfdc;">&nbsp;<b>{password}</b>&nbsp;</td></tr><tr><td colspan="3" style="border-bottom-left-radius:12px;border-bottom-right-radius:12px;">&nbsp;</td></tr></table></td></tr></table><br /><br /><hr width="80%" align="left" size="1" /><br />We recommend you change your password immediately after logging in for the first time.<br />Click on {menupath} in the navigation menu and change your password here.
		// Der Administrator {admin} hat f&uuml;r Ihr Benutzerkonto in <b>fx-project</b> ein <b style="color:#009f6b;">neues</b> Passwort generiert.^<br />Die Sperre Ihres Benutzerkontos in <b>fx-project</b> wurde <b style="color:#009f6b;">aufgehoben</b>.^<br /><br /><hr width="80%" align="left" size="1" /><br />Bitte verwenden Sie nachfolgende Informationen um sich in <b>fx-project</b> anzumelden:<br /><br /><br /><table cellpadding="1" cellspacing="1" bgcolor="#666666" style="border-radius:12px;box-shadow:8px 8px 8px #666666;"><tr><td bgcolor="#fafafa" style="border-radius:12px;"><table cellpadding="2" cellspacing="1" style="border-radius:12px;"><tr><td colspan="3" bgcolor="#ececec" style="border-top-left-radius:12px;border-top-right-radius:12px;">&nbsp;&nbsp;<b style="font-size:16pt;color:#666666;">Benutzerlogin</b></td></tr><tr><td colspan="3">&nbsp;</td></tr><tr><td align="right">&nbsp;&nbsp;<font style="color:#8f788f;">Mandanten-Nr.:</font>&nbsp;</td><td width="150" style="border:1px solid #b5cfdc;">&nbsp;<b>{client}</b>&nbsp;</td><td rowspan="3">&nbsp;&nbsp;</td></tr><tr><td align="right">&nbsp;&nbsp;<font style="color:#8f788f;">Benutzername:</font>&nbsp;</td><td style="border:1px solid #b5cfdc;">&nbsp;<b>{uname}</b>&nbsp;</td></tr><tr><td align="right">&nbsp;&nbsp;<font style="color:#8f788f;">Passwort:</font>&nbsp;</td><td style="border:1px solid #b5cfdc;">&nbsp;<b>{password}</b>&nbsp;</td></tr><tr><td colspan="3" style="border-bottom-left-radius:12px;border-bottom-right-radius:12px;">&nbsp;</td></tr></table></td></tr></table><br /><br /><hr width="80%" align="left" size="1" /><br />Wir empfehlen Ihnen sofort nach dem ersten Anmelden Ihr Passwort abzu&auml;ndern.<br />Klicken Sie dazu im Navigationsmen&uuml; auf {menupath} und &auml;ndern Sie hier Ihr Passwort ab.
		$md='email_pwd_changed';
		if(!isset($GLOBALS['_ma'][$md.'_'.$lang]))
			$GLOBALS['_ma'][$md.'_'.$lang]=explode('^',get_message($md));

		$title=meldung(1234);	// fx-project Login Data
		$ldrepa=array(
			'{client}'		=> $GLOBALS['fxpglobals']['client'],
			'{uname}'		=> $uda['name_benutzer'],
			'{password}'	=> $npw,
			'{menupath}'	=> '<b style="color:#'.colorCode('darkgrey').';">'.trPath(67).'</b>'
		);
		$data=strtr($GLOBALS['_ma'][$md.'_'.$lang][0], array('{admin}'=>'(<b style="color:#'.colorCode('grey').';">'.$GLOBALS['_ua'][$sender]['name2'].'</b>)'));
		if($uda['status_benutzer'] == FXP_USER_LOCKED)
			$data .= $GLOBALS['_ma'][$md.'_'.$lang][1];
		$data .= strtr($GLOBALS['_ma'][$md.'_'.$lang][2],$ldrepa);

		$GLOBALS['_ea']=array(
			$recipient => array(
				'email'	=>$GLOBALS['_ua'][$recipient]['email'],
				$md		=> array(
					'title'	=> $title,
					'data'	=> array(0=>$data)
				)
			)
		);

		$sc=sendPersonMails($GLOBALS['_ea'],$GLOBALS['_ua']);

		$GLOBALS['fxpglobals']['settings']=$settings_backup;
		$GLOBALS['fxpglobals']['lang']=$lang_backup;

		if(!$sc)
			return 1166;	// User status and password for "$var1" could not be changed.
	}

	return 1165;	// User status and password for "$var1" have been changed successfully.
}

/**
 * delAttachments
 * Diese Funktion löscht alle E-Mail-Anhangsdateien
 * -----------------------------------------------------------------------------
 * Autor: Walter_T
 *
 * @return ???RETURN???
 */
function delAttachments()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$del=0;
	if(fxIsArray($GLOBALS['mail_attachments']))
	{
		foreach($GLOBALS['mail_attachments'] as $ma_filepath => $ma_array)
		{
			if(file_exists($ma_filepath))
			{
				unlink($ma_filepath);
				$del++;
			}
		}
		$GLOBALS['mail_attachments']=array();
	}

	return $del;
}

/**
 * ZEITERFASSUNG
 * addiert auf den übergebenen Vorgang ($Projekt_ID) und all seinen Eltern einen
 * übergebenen Aufwand-Ist und die daraus resultierenden Kosten.
 * Aufwand_ToDo wird ebenfalls berücksichtigt.
 * -----------------------------------------------------------------------------
 * $Projekt_ID: 	einschliesslich ab diesem Vorgang werden die Daten nach oben
 * weitergereicht
 * $Zeitaufwand:	zu addierender Aufwand-Ist in Sekunden
 * -----------------------------------------------------------------------------
 * Author: 			Stefan + wtj
 *
 * @param various $Projekt_ID         - Mandatory parameter:                   ???PARAMETER???
 * @param integer $Zeitaufwand        - Optional parameter (default = 0):      ???PARAMETER???
 * @param boolean $meldung            - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $ignore_zeitaufwand - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function Zeiterfassung($Projekt_ID, $Zeitaufwand=0, $meldung=false, $ignore_zeitaufwand=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$Projekt_ID=(int)$Projekt_ID;
	if(!$Projekt_ID)
		return false;

	// Zeitaufwand auf alle übergeordneten Vorgänge hoch addieren
	$Zeitaufwand=(int)$Zeitaufwand;
	if($Zeitaufwand != 0)
	{
		$where_sql	= '';
		$todo_sql	= '';

		// Ist der Vorgang ein ToDo? => ToDo-Zeitaufwand ebenfalls hoch addieren
		$sql="SELECT vorgangsnummer, projektart FROM projekte WHERE projekt_id=".$Projekt_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$dat=db_values($sql);
		if(fxIsArray($dat))
		{
			// ToDo
			if((int)$dat['projektart'] == 79)
				$todo_sql=", aufwand_todo=(aufwand_todo + ".$Zeitaufwand.")";

			// Alle Vorgangsnummern
			$where_sql=" mandanten_id=".$GLOBALS['fxpglobals']['client']." AND (vorgangsnummer='".$dat['vorgangsnummer']."'";
			$vnt=substr($dat['vorgangsnummer'], 0, -5);
			while(strlen($vnt) > 3)
			{
				$where_sql .= " OR vorgangsnummer='".$vnt."'";

				$vnt=substr($vnt, 0, -5);
			}
			$where_sql .= ")";
		}

		if(strlen($where_sql))
		{
			$sql="UPDATE projekte SET aufwand_ist=(aufwand_ist + ".$Zeitaufwand.")".$todo_sql." WHERE".$where_sql;
			$err=db_x($sql, $meldung, false, false);
//echo('Zeitaufwand-SQL:<br />'.$sql.'<hr />');
		}
	}

	// Budgetanpassung durchführen
	if(!$err)
		$err=update_budget($Projekt_ID, array('budget_int_verbr'=>true));

	return $err;
}

/**
 * -----------------------------------------------------------------------------
 * ------------------------------------------------------------------ CHECKDATUM
 * -----------------------------------------------------------------------------
 *
 * @param various $cd_datum - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function checkdatum($cd_datum)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$datum=$GLOBALS[$cd_datum];
	if(!strlen($datum))
		$datum=$_POST[$cd_datum];

	$cd_err=false;
	$cd_neu=validate_entry($datum, $cd_err, 'datum', '0');
	// Datum o.k.
	if(!$cd_err)
		return $cd_neu;

	// Datum fehlerhaft
	error_msg($cd_err, false, $cd_datum);
	$GLOBALS['fehler']	= true;
	$GLOBALS['err']		= $cd_err;

	return '^'.$datum;
}

/**
 * -----------------------------------------------------------------------------
 * ------------------------------------------------------------------- FILE_SIZE
 * -----------------------------------------------------------------------------
 *
 * @param various $zahl - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function file_size($zahl)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$zahl=(int)$zahl;
	$nk=$GLOBALS['fxpglobals']['settings']['nkstellen'];
	if(!$nk)
		$nk=2;

	if($zahl > 1073741824)		// GigaByte
		$zahl=round(($zahl / 1073741824), $nk).' GB';
	else if($zahl > 1048576)	// MegaByte
		$zahl=round(($zahl / 1048576), $nk).' MB';
	else if($zahl > 1024)		// KiloByte
		$zahl=round(($zahl / 1024), $nk).' KB';
	else if($zahl)				// Byte
		$zahl=$zahl.' B';
	else
		$zahl='???';	// meldung(2004);	// unbekannt

	return $zahl;
}

/**
 * ???FUNCTION???
 *
 * @param various $txt_string - Mandatory parameter:                ???PARAMETER???
 * @param string  $trnr       - Optional parameter (default = ''):  ???PARAMETER???
 * @param string  $mzs        - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function extract_lit($txt_string, $trnr='', $mzs='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($txt_string))
		return '';

	$larray=array();
	while(strlen($txt_string))
	{
		$lp=strpos($txt_string,'#lit');
		if($lp !== false)
		{
			$bp=strpos($txt_string,"\n",$lp+4);
			if($bp !== false)
			{
				$line=trim(substr($txt_string,$lp+4,$bp-$lp-4));
				$txt_string=trim(substr($txt_string,$bp+1));
			}
			else
			{
				$line=trim(substr($txt_string,$lp+4));
				$txt_string='';
			}

			if(strlen($line))
			{
				$la=array();
				$lt='';
				$ll='';
				for($i=0; $i<strlen($line); $i++)
				{
					$c=substr($line,$i,1);
					if(($c == '_') || ((ord($c) >= 0x30) && (ord($c) <= 0x39)))	// Transaktionsnummer im Format # oder #_#
						$lt .= $c;
					else
					{
						if(strlen($lt))
						{
							$la[$lt]='';
							$lt='';
						}
						if($c != '|')
						{
							$ll=trim(substr($line,$i));
							break;
						}
					}
				}
				if(strlen($ll) && sizeof($la))
				{
					foreach($la as $lt => $dummy)
						$larray[$lt]=$ll;
				}
			}
		}
		else
			$txt_string='';
	}

	$lit='';
	if(fxIsArray($larray))
	{
		if($trnr)
			$search=(string)$trnr;
		else
			$search=(string)$GLOBALS['fxpglobals']['tr'];

		// Suche nach #tr???_??? bzw. #tr???
		if(strlen($mzs))
			$sa=array($search.'_'.$mzs, $search);
		else
			$sa=array($search);

		foreach($sa as $search)
		{
			if(isset($larray[$search]))
			{
				$lit=$larray[$search];
				break;
			}
		}
	}

	return $lit;
}

/**
 * extract_help
 * Mit Hilfe dieser Funktion wird ein Hilfetext durchsucht und der richtige Text
 * für die aktuelle Transaktion zurückgeliefert.
 * -----------------------------------------------------------------------------
 * Variablen:
 * °°°°°°°°°°
 * $txt_string	->	Hilfetext aus der Datenbank, der durchsucht werden soll.
 * $trnr		->	(optional) Transaktionsnummer zum herausfiltern.
 * Rückgabe:
 * °°°°°°°°°
 * Gefundener Hilfetext.
 * -----------------------------------------------------------------------------
 * Autor: Walter_T
 *
 * @param various $txt_string - Mandatory parameter:                ???PARAMETER???
 * @param string  $trnr       - Optional parameter (default = ''):  ???PARAMETER???
 * @param string  $mzs        - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function extract_help($txt_string, $trnr='', $mzs='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($txt_string))
		return '';

	$txt_trans=array("<BR>"=>'<br />', "<BR />"=>'<br />', "<br>"=>'<br />', "\r\n"=>'<br />', "\r"=>'<br />', "\n"=>'<br />', '&quot;'=>'"');
	$txt_string=strtr($txt_string,$txt_trans);

	// Alle Literalzeilen eliminieren
	$tarray=array();
	while(strlen($txt_string))
	{
		$lp=strpos($txt_string,'#lit');
		if($lp !== false)
		{
			if($lp > 0)
				$tarray[]=trim(substr($txt_string,0,$lp));
			$bp=strpos($txt_string,"\n",$lp+4);
			if($bp !== false)
				$txt_string=trim(substr($txt_string,$bp+1));
			else
				$txt_string='';
		}
		else
		{
			$tarray[]=trim($txt_string);
			$txt_string='';
		}
	}
	if(fxIsArray($tarray))
	{
		$txt_string='';
		foreach($tarray as $t)
		{
			if(strlen($txt_string))
				$txt_string .= '<br />';
			$txt_string .= $t;
		}
	}

	if(!strlen($txt_string))
		return '';

	if($trnr)
		$search=(string)$trnr;
	else
		$search=(string)$GLOBALS['fxpglobals']['tr'];

	$fnd=-1;

	// Suche nach #tr???_??? bzw. #tr???
	if(strlen($mzs))
		$sa=array($search.'_'.$mzs, $search);
	else
		$sa=array($search);
	foreach($sa as $search)
	{
		$tarray=explode('#tr',$txt_string);
		foreach($tarray as $cnt => $text)
		{
			if(!strlen($text))
				continue;
//fxDebug($text, '<b>Suche nach:</b> '.$search);
			// Startposition des Textes ermitteln
			for($spos=0; $spos<strlen($text); $spos++)
			{
				$az=ord(substr($text,$spos,1));
				if((($az < 0x30) || ($az > 0x39)) && ($az != 0x7C) && ($az != 0x5F)) // Zeichen nicht 0-9, |, _
					break;
			}
			$str=substr($text,0,$spos);
			$tarray[$cnt]=substr($text, $spos);
//fxDebug($tarray[$cnt], $str);
			if(strlen($str))
			{
				$strex=explode('|', $str);
				foreach($strex as $txt)
				{
					if($txt == $search)
					{
						$fnd=$cnt;
						break;
					}
				}
			}
			if($fnd >= 0)
				break;
		}
		if($fnd >= 0)
			break;
	}
	if($fnd < 0)
		$fnd=0;

	while(substr($tarray[$fnd],0,6) == '<br />')
		$tarray[$fnd]=substr($tarray[$fnd],6);

//echo('<b>Rückgabe:</b><hr />'.fxHtmlEncode($tarray[$fnd]).'<hr />');
//fxDebug($tarray[$cnt], $trnr);
	return extract_meldung($tarray[$fnd], true, $trnr);
}

/**
 * extract_meldung
 * Mit Hilfe dieser Funktion wird ein Hilfetext durchsucht und sämtliche
 * Meldungen ersetzt, sowie transaktionsspezifische Ersetzungen vorgenommen.
 * -----------------------------------------------------------------------------
 * Variablen:
 * °°°°°°°°°°
 * $txt_string	->	Hilfetext, der durchsucht werden soll.
 * Rückgabe:
 * °°°°°°°°°
 * Ersetzter Hilfetext.
 * -----------------------------------------------------------------------------
 * Autor: Walter_T
 *
 * @param various $txt_string   - Mandatory parameter:                  ???PARAMETER???
 * @param boolean $txt_all      - Optional parameter (default = true):  ???PARAMETER???
 * @param string  $trnr         - Optional parameter (default = ''):    ???PARAMETER???
 * @param integer $maxwidth_pic - Optional parameter (default = 0):     ???PARAMETER???
 *
 * @return ???RETURN???
 */
function extract_meldung($txt_string, $txt_all=true, $trnr='', $maxwidth_pic=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(strlen($txt_string) == 0)
		return '';

	$txt_trans=array("\r\n"=>'<br />', "\r"=>'<br />', "\n"=>'<br />');
	$txt_string=strtr($txt_string, $txt_trans);

	$trshown=false;
	if(!$trnr)
		$trnr=$GLOBALS['fxpglobals']['tr'];

	// Meldungen
	while(true)
	{
		$sp=strpos($txt_string, '$meldung_');
		if($sp !== false)
		{
			$ep=strpos($txt_string, ';', $sp);
			if($ep === false)
				$ep=strlen($txt_string);
			$extract=substr($txt_string, $sp+9, $ep-$sp-9);
//fxDebug($extract, 'Extracted String', 0);
			$mex=explode(',', $extract);
			if(fxIsArray($mex))
			{
//fxDebug($mex, 'Exploded Extraction');
				$rint=0;
				if(isset($GLOBALS['intern']) && $GLOBALS['intern'])
				{
					$rint=$GLOBALS['intern'];
					unset($GLOBALS['intern']);
				}
				$mstr='$mldg=meldung('.(int)$mex[0];
				$som=sizeof($mex);
				if($som == 1)
					$mstr .= ', true';
				else
				{
					$mstr .= ', false';
					for($mi=1; $mi<sizeof($mex); $mi++)
					{
						$mw=(int)$mex[$mi];
						if(($mex[$mi] == $mw) && (strlen($mex[$mi]) == strlen($mw)))
							$mex[$mi]='^'.meldung($mw, true);
						if(substr($mex[$mi], 0, 1) == '$')
							$mex[$mi]='\\'.$mex[$mi];
						$mstr .= ', "'.$mex[$mi].'"';
					}
				}
				$mstr .= ');';
//fxDebug($mstr, 'Converted Exploded Extraction');
				eval($mstr);
				if($rint)
					$GLOBALS['intern']=$rint;
				if(!strlen($mldg))
				{
					if(!$trshown && $GLOBALS['help_showmessages'])
					{
						$trshown=true;
						echo('<hr /><b><u>TRANSAKTION: '.$trnr.'</u></b><br /><br />'.$GLOBALS['nl']);
					}
					$mldg='<b>#### Meldung fehlt: '.fxHtmlEncode($mstr).' ####</b>';
					if($GLOBALS['help_showmessages'])
						echo($mldg.'<br />'.$GLOBALS['nl']);
				}
				else if($rint)
					$mldg='<span class="dintl" title="wertetab: wertetabid='.$mex[0].', id_sprache='.$GLOBALS['fxpglobals']['lang'].'" onclick="window.open(\'language.php?mfield=tabwert&slng='.$rint.'&dlng='.$GLOBALS['fxpglobals']['lang'].'&pks_wertetabid='.$mex[0].fxReturnSessionParams().'\');">'.$mldg.'</span>';
//fxDebug($mldg, 'Result');
				$txt_string=substr($txt_string, 0, $sp).$mldg.substr($txt_string, $ep+1).' ';
			}
		}
		else
			break;
	}

	// Hilfetextbeschreibung
	if($txt_all)
	{
		// --- Transaktions-, Masken-, Hilfetext- + Feldbezeichnung
		$col_t='005077'; $col_m='5a5a5a'; $col_f='5a5a5a'; $col_h='5a5a5a';
		$search=array(
			array('$tnr',	'',			'',			'<font class="manual_bld" color="#'.$col_t.'">',	'</font>',	6,	'text_kurz'),
			array('$mnr',	'',			'',			'<font class="manual_bld" color="#'.$col_m.'">',	'</font>',	2,	'text_kurz'),
			array('$fnr',	'',			'',			'<font class="manual_bld" color="#'.$col_f.'">',	'</font>',	3,	'literal_20'),
			array('$hnr',	'&quot;',	'&quot;',	'<font class="manual_bld" color="#'.$col_h.'">',	'</font>',	0,	'tabwert')
		);
		for($tmhf=0; $tmhf<4; $tmhf++)
		{
			while(true)
			{
				$sp=strpos($txt_string, $search[$tmhf][0]);
				if($sp !== false)
				{
					$nr='';
					$snr='';
					$add=0;
					$ep=$sp+4;
					while($ep < strlen($txt_string))
					{
						$ac=substr($txt_string, $ep, 1);
						if($ac == '_')
						{
							$add=1;
							$ep++;
						}
						else if(is_numeric($ac) || ($ac == '-'))
						{
							if($add)
								$snr .= $ac;
							else
								$nr .= $ac;
							$ep++;
						}
						else
							break;
					}
					$ersatz='';
					if($nr)
					{
						$mmnr=substr($search[$tmhf][0], 0, 1).$nr.'_'.$snr;
						if(isset($GLOBALS['memory'][$mmnr.'_'.$GLOBALS['fxpglobals']['lang']]))
							$ersatz=$GLOBALS['memory'][$mmnr.'_'.$GLOBALS['fxpglobals']['lang']];
						else
						{
							if(!$search[$tmhf][5])			// Hilfetext
								$ersatz=db_value("SELECT ".$search[$tmhf][6]." FROM wertetab WHERE wertetabid=".$nr." AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")");
							else if($search[$tmhf][5] == 6)	// Transaktionsname
							{
								$ersatz=trPath((int)$nr);
//echo('Transaktionsname von tr='.$nr.' inkl. Pfad: <b>'.$ersatz.'</b><br />'.$GLOBALS['nl']);
							}
							else
							{
								$ssql='';
								if(!strlen($ersatz))
								{
									$ssql="SELECT ".$search[$tmhf][6]." FROM texte WHERE id_referenz=".$nr." AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND id_ref_art=".$search[$tmhf][5];
									$ersatz=db_value($ssql);
									if(!$ersatz)
									{
										if($search[$tmhf][5] == 3)	// Felder
										{
											$asql="SELECT alias_zu FROM felder WHERE id=".$nr." AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")";
											$alias_nr=(int)db_value($asql);
											if($alias_nr)
											{
												$ssql="SELECT ".$search[$tmhf][6]." FROM texte WHERE id_referenz=".$alias_nr." AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND id_ref_art=".$search[$tmhf][5];
												$ersatz=db_value($ssql);
											}
										}
									}
								}
							}
							if(strlen($snr))
							{
								$hl=explode('°', $ersatz);
								$ersatz=$hl[(int)$snr];
							}
							if($ersatz)
								$ersatz=$search[$tmhf][1].$ersatz.$search[$tmhf][2];
							else
							{
								$ersatz='<b>#### $ '.substr($search[$tmhf][0], 1).$nr;
								if(strlen($snr))
									$ersatz .= '_'.$snr;
								$ersatz .= ' nicht gefunden ####</b>';
								if(!$trshown && $GLOBALS['help_showmessages'])
								{
									$trshown=true;
									echo('<hr class="fxhr"><b><u>Transaktion: '.$trnr.'</u></b><br /><br />');
								}
								if($GLOBALS['help_showmessages'])
								{
									if($ssql)
										echo('SQL: &quot;'.$ssql.'&quot;<br />');
									echo($ersatz.'<br />');
								}
							}
							$GLOBALS['memory'][$mmnr.'_'.$GLOBALS['fxpglobals']['lang']]=$ersatz;
						}
						$ersatz=$search[$tmhf][3].$ersatz.$search[$tmhf][4];
					}
					else
					{
						$ersatz='<b>#### $ '.substr($search[$tmhf][0], 1).$nr;
						if(strlen($snr))
							$ersatz .= '_'.$snr;
						$ersatz .= ' fehlt ####</b>';
						if(!$trshown && $GLOBALS['help_showmessages'])
						{
							$trshown=true;
							echo('<hr class="fxhr"><b><u>Transaktion: '.$trnr.'</u></b><br /><br />');
						}
						if($GLOBALS['help_showmessages'])
							echo($ersatz.'<br />');
					}
					$txt_string=substr($txt_string, 0, $sp).$ersatz.substr($txt_string, $ep);
				}
				else
					break;
			}
		}

		// --- KURZINFO, MASKENINFO, BESCHREIBUNG + HINWEIS
		$info=array(
			array('ki',	false,	strtoupper(meldung(1557, true))),	// KURZINFO
			array('mi',	'li',	strtoupper(meldung(1558, true))),	// MASKENINFO
			array('be',	'li',	strtoupper(meldung(1559, true))),	// BESCHREIBUNG
			array('hi',	'lh',	strtoupper(meldung(1561, true)))	// HINWEIS
		);

		$start_headline=false;
		for($kmbh=0; $kmbh<4; $kmbh++)
		{
			$headline=false;
			if($GLOBALS['headline_set'])
				$headline=true;
			$lp=0;
			while(true)
			{
				$sp=strpos($txt_string, '<'.$info[$kmbh][0].'>');
				if($sp !== false)
				{
					$ep=strpos($txt_string, '</'.$info[$kmbh][0].'>', $sp);
					if($ep === false)
						$ep=strlen($txt_string);
					$extract=substr($txt_string, $sp+4, $ep-$sp-4);
					if(!$headline)
					{
						$ersatz='';
						if($start_headline || ($sp > 0))
							$ersatz='<br />';

						$ersatz .= '<b class="s4">'.$info[$kmbh][2].':</b><br />';	// Titelzeile
						$start_headline=true;
						$headline=true;
					}
					else if($lp)
						$ersatz='<br />';
					else
						$ersatz='';
					if($info[$kmbh][0] == 'mi')	// Maskeninfo speziell behandeln
					{
						$pp=strpos($extract, '|');
						if($pp !== false)
						{
							$mt=substr($extract, 0, $pp);
							$mi=substr($extract, $pp+1);
						}
						else
						{
							$mt=$extract;
							$mi='';
						}
						$mt=str_replace('#'.$col_m, '#000000', $mt);
						$ersatz .= '<div class="manual_std"><img src="'.$GLOBALS['gfxpath'].$info[$kmbh][1].'.png" align="center"><b>'.$mt.'</b></div>';
						if($mi)
							$ersatz .= '<br /><div class="manual_std">'.$mi.'</div>';
					}
					else
						$ersatz .= $extract;
					if(strtolower(substr($txt_string, $ep+5,6)) == '<br />')
						$ep += 6;
					$txt_string=substr($txt_string, 0,$sp).$ersatz.substr($txt_string, $ep+5);
					$lp=$sp+strlen($ersatz);
				}
				else
				{
					if($lp)
						$txt_string=substr($txt_string, 0,$lp).'<br />'.substr($txt_string, $lp);
					break;
				}
			}
		}
	}

	// Bilder ersetzen
	$txt_string=str_replace('{GFX}',$GLOBALS['gfxpath'],$txt_string);

	$picpos=strpos($txt_string, '<pic>');
	while($picpos !== false)
	{
		$endpicpos2=0;
		$endpicpos=strpos($txt_string,'</pic>',$picpos);
		if($endpicpos !== false)
			$endpicpos2=$endpicpos+6;
		else
		{
			$endpicpos=strpos($txt_string,' ',$picpos);
			if($endpicpos !== false)
				$endpicpos2=$endpicpos+1;
		}
		if(!$endpicpos2)
			break;

		$obildname=strtolower(substr($txt_string, $picpos+5, $endpicpos-$picpos-5));
		$bildname=$obildname;
		$width='';
		$style='';
		$pp=strpos($bildname,'|');
		if($pp !== false)
		{
			$aa=strtolower(substr($bildname,$pp+1,1));
			$at=substr($bildname,$pp+2);
			$bildname=substr($bildname,0,$pp);
			if($aa == 's')	// Style
				$style .= $at;
			else
				$width=' width="'.$at.'"';
		}
		$ptype='';
		if(substr($bildname, -4, 1) == '.')
		{
			$ptype=strtolower(substr($bildname,-3));
			$bildname=substr($bildname,0,-4);
		}
		$pfad_arr=array($GLOBALS['gfxpath'], $GLOBALS['docpath'].substr('00'.$GLOBALS['fxpglobals']['lang'],-2).'/');

		// Bild checken
		if(strlen($ptype))
			$ea=array($ptype);
		else
			$ea=array('png','jpg','gif');
		$img='';
		$est='';
		foreach($pfad_arr as $pfad)
		{
			foreach($ea as $pt)
			{
				$src=$pfad.$bildname.'_'.$GLOBALS['fxpglobals']['lang'].'.'.$pt;
				if(!file_exists($src))
				{
					if(strlen($est))
						$est .= $GLOBALS['nl'];
					$est .= $src;
					$src=$pfad.$bildname.'.'.$pt;
				}
				if(file_exists($src))
				{
					$iwidth='';
					if(strlen($width))
						$iwidth=$width;
					else if($maxwidth_pic > 0)
					{
						$ims=GetImageSize($src);
						if(fxIsArray($ims))
							$iwidth=' width="'.min($ims[0],$maxwidth_pic).'"';
					}
					$img='<img src="'.$src.'"'.$iwidth.' align="middle" style="vertical-align:middle;'.$style.'">';
					break;
				}
				else
				{
					if(strlen($est))
						$est .= $GLOBALS['nl'];
					$est .= $src;
				}
			}
			if(strlen($img))
				break;
		}

		if(!strlen($img))
		{
			$img=' <b class="red" title="'.$est.'">#### Bild ['.fxHtmlEncode($obildname).'] fehlt! ###</b> ';
			if(!$trshown && $GLOBALS['help_showmessages'])
			{
				$trshown=true;
				echo('<hr /><b><u>TRANSAKTION: '.$trnr.'</u></b><br /><br />'.$GLOBALS['nl']);
			}
			if($GLOBALS['help_showmessages'])
				echo($img.'<br />'.$GLOBALS['nl']);
		}

		$txt_string=substr($txt_string,0,$picpos).$img.substr($txt_string,$endpicpos2);
//echo('Bild [<b>'.fxHtmlEncode($pfad.$bildname).'</b>] an Pos. '.$picpos.' ersetzt...<br />'.$GLOBALS['nl']);
		$picpos=strpos($txt_string,'<pic>',$picpos);
	}

	// Prüfen, ob es die Bilder gibt
	if($GLOBALS['help_showmessages'])
	{
		$itsp=0;
		$imagetagfound=stripos($txt_string,' src=',$itsp);
		while($imagetagfound)
		{
			$itsp=$imagetagfound+6;
			$eit=strpos($txt_string,'"',$itsp);
			if($eit)
			{
				$bildpfad=substr($txt_string,$itsp,$eit-$itsp);
				if(!file_exists($bildpfad))
				{
					if(!$trshown)
					{
						$trshown=true;
						echo('<hr /><b><u>TRANSAKTION: '.$trnr.'</u></b><br /><br />'.$GLOBALS['nl']);
					}
					echo('<b class="red">#### Zusatzbild ['.fxHtmlEncode($bildpfad).'] fehlt! ####</b><br />'.$GLOBALS['nl']);
				}
				$itsp=$eit+1;
			}
			$imagetagfound=stripos($txt_string,' src=',$itsp);
		}
	}
//echo('<pre>'.fxHtmlEncode($txt_string).'</pre>');

	return $txt_string;
}

/**
 * ???FUNCTION???
 *
 * @param various $trans - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function trPath($trans)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$trans=(int)$trans;
	$lnk='';

	// Load Menu and extract transaction link and path
	$ma=loadMenu(true);
	if(fxIsArray($ma))
	{
		foreach($ma as $tk => $tc)
		{
			if($tc['tr'] == $trans)
			{
				$lnk=$tc['tx'];
				while(strlen($tk) > 3)
				{
					$tk=substr($tk,0,-3);
					$lnk=$ma[$tk]['tx'].'&gt;'.$lnk;
				}
				break;
			}
		}
	}

	if(!strlen($lnk))
		$lnk='TR '.$trans;

	return $lnk;
}

/**
 * get_double
 * Mit Hilfe dieser Funktion wird ein beliebiger Ausdruck in einen Double-Wert
 * umgewandelt.
 * -----------------------------------------------------------------------------
 * Autor: Walter_T
 *
 * @param various $bvalue - Mandatory parameter:                ???PARAMETER???
 * @param string  $ktz    - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_double($bvalue, $ktz='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($bvalue))
		$rvalue=(double)0.00;
	// Sonderfall, falls wissenschaftliche Notation wie z.B. 1.000000E-14 oder 2.89588232E+17
	else if(strpos($bvalue,'E-') || strpos($bvalue,'E+'))
		$rvalue=db_double($bvalue);
	else if(is_int($bvalue) || is_float($bvalue))
		$rvalue=(double)$bvalue;
	else
	{
		$rvk='';
		$rnk='';
		$neg=false;
		$nk=false;
		for($i=0; $i<strlen($bvalue); $i++)
		{
			$b=substr($bvalue, $i, 1);
			if(($b == ',') || ($b == '.'))
			{
				if($ktz)
				{
					if(!$nk && ($b == $ktz))
						$nk=true;
				}
				else
				{
					if($nk)
					{
						$rvk .= $rnk;
						$rnk='';
					}
					$nk=true;
				}
			}
			else if($b == '-')	// neg. Zahlen
				$neg=true;
			else if(is_numeric($b))
			{
				if($nk)
					$rnk .= $b;
				else
					$rvk .= $b;
			}
		}
		if(!$rvk)
			$rvk='0';
		if(!$rnk)
			$rnk='00';
		$rvalue=(double)($rvk.'.'.$rnk);
		if($neg)
			$rvalue=-1.00 * $rvalue;
	}

//echo($bvalue.' &gt; '.$rvalue.'<br />');
	return $rvalue;
}

/**
 * ???FUNCTION???
 *
 * @param various $value     - Mandatory parameter:               ???PARAMETER???
 * @param integer $precision - Optional parameter (default = 4):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function db_double($value, $precision=4)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	//$precision=ini_get('serialize_precision');
	$rv=rtrim(sprintf('%.'.$precision.'f', $value), '0');
	if(substr($rv,-1) == '.')
		$rv .= '0';
	return $rv;
}

/**
 * ???FUNCTION???
 *
 * @param various $value - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function db_money($value)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return db_double(round($value, 2), 2);
}

/**
 * rechte_referenz
 * (!! NICHT FÜR DIE PROJEKTE (Berücksichtigt nicht die Unterprojekte) !!)
 * Funktion liefert array mit boolean Aktionsrechten
 * Zu übergebende Variablen:
 * °°°°°°°°°°°°°°°°°°°°°°°°°
 * $refnr:	Zu welcher Referenz werden Rechte abgefragt (35: Report, ...)
 * $id:		ID der Referenz (RepID, ...)
 * $bid:	<optional>	Benutzer_ID, dessen Rechte geprüft werden
 * $mid:	<optional>	Mandanten_ID
 * $mode:	1->Array zurückliefern, sonst Wert
 *
 * @param various $refnr - Mandatory parameter:                ???PARAMETER???
 * @param various $id    - Mandatory parameter:                ???PARAMETER???
 * @param string  $bid   - Optional parameter (default = ''):  ???PARAMETER???
 * @param string  $mid   - Optional parameter (default = ''):  ???PARAMETER???
 * @param integer $mode  - Optional parameter (default = 1):   ???PARAMETER???
 *
 * @return ???RETURN???
 */
function rechte_referenz($refnr, $id, $bid='', $mid='', $mode=1)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!$refnr || !$id)
		return false;

	if(!$bid)
		$bid=$GLOBALS['fxpglobals']['user'];
	if(!$mid)
		$mid=$GLOBALS['fxpglobals']['client'];

	$bit		= array(1,2,4,8,16,32,64,128);
	$rechtea	= '';
	$rechte		= 0;

	$sql  = "SELECT aktionscode, gruppeart, benutzergr_id AS id FROM rechte_referenz";
	$sql .= " WHERE rechte_ref=".$refnr." AND id_referenz=".$id." AND mandanten_id=".$mid;
	$rar =db_values($sql, '*');
//fxDebug($rar, $sql);

	if(fxIsArray($rar))
	{
		// Abteilungen des Benutzer ermitteln
		if(!fxIsArray($GLOBALS['glob_abt'][$bid]))
		{
			$pid=(int)db_value("SELECT personen_id FROM benutzer WHERE benutzer_id=".$bid." AND mandanten_id=".$mid);
			$GLOBALS['glob_abt'][$bid]=get_abteilungen($pid, $mid);
			if(!fxIsArray($GLOBALS['glob_abt'][$bid]))
				$GLOBALS['glob_abt'][$bid][0]=false;
//fxDebug($GLOBALS['glob_abt'][$bid], 'Abteilungen');
		}
		// Benutzergruppen des Benutzer ermitteln
		if(!fxIsArray($GLOBALS['glob_bgr'][$bid]))
		{
			$GLOBALS['glob_bgr'][$bid]=get_benutzergruppen($bid, $mid);
			if(!fxIsArray($GLOBALS['glob_bgr'][$bid]))
				$GLOBALS['glob_bgr'][$bid][0]=false;
//fxDebug($GLOBALS['glob_bgr'][$bid], 'Benutzergruppen');
		}
		// Projekte des Benutzer ermitteln
		if(!fxIsArray($GLOBALS['glob_prj'][$bid]))
		{
			$prjsql=prights_get(0);
			if(strlen($prjsql))
			{
				$sql="SELECT projekt_id FROM projekte WHERE projekt_id IN (".$prjsql.")";
				$arr=db_values($sql, '*');
				if(fxIsArray($arr))
				{
					foreach($arr as $a)
						$GLOBALS['glob_prj'][$bid][(int)$a['projekt_id']]=true;
				}
				else
					$GLOBALS['glob_prj'][$bid][0]=false;
			}
			else
				$GLOBALS['glob_prj'][$bid][0]=false;
//fxDebug($GLOBALS['glob_prj'][$bid], 'Projekte');
		}
		foreach($rar as $r)
		{
			switch($r['gruppeart'])
			{
				case 2163:	// Benutzer
					if($r['id'] == $bid)
						$rechte |= $r['aktionscode'];
				break;

				case 2164:	// Benutzergruppe
					if($GLOBALS['glob_bgr'][$bid][$r['id']])
						$rechte |= $r['aktionscode'];
				break;

				case 2165:	// Abteilung
					if($GLOBALS['glob_abt'][$bid][$r['id']])
						$rechte |= $r['aktionscode'];
				break;

				case 1920:	// Projekte
					if($GLOBALS['glob_prj'][$bid][$r['id']])
						$rechte |= $r['aktionscode'];
				break;
			}
		}
	}

	// Keine Rechte vorhanden, dann false zurückgeben
	if(!$rechte)
		return false;

	// Rechte als Zahl zurückgeben
	if($mode != 1)
		return $rechte;

	// Rechte als Array zurückgeben
	for($b=0; $b<8; $b++)
	{
		if($rechte & $bit[$b])
			$rechtea[$b]=true;
		else
			$rechtea[$b]=false;
	}
	return $rechtea;
}

/**
 * ???FUNCTION???
 *
 * @param integer $u   - Optional parameter (default = 0):     ???PARAMETER???
 * @param boolean $all - Optional parameter (default = false): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getAllProgramRights($u=0, $all=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	//echo('function <b>getAllProgramRights</b>($u='.$u.', $all='.$all.')<br />');
	$prgrights=array();

	// Get client, person and user id
	$c=$GLOBALS['fxpglobals']['client'];
	$u=(int)$u;
	if(!$u)
	{
		$p=$GLOBALS['fxpglobals']['person'];
		$u=$GLOBALS['fxpglobals']['user'];
	}
	else if($u < 0)
	{
		$p=(int)abs($u);
		$u=(int)db_value("SELECT benutzer_id FROM benutzer WHERE personen_id=".$p." AND mandanten_id=".$c." AND (status_benutzer=0 OR status_benutzer IS NULL) ORDER BY punkte DESC");
	}
	else
		$p=(int)db_value("SELECT personen_id FROM benutzer WHERE benutzer_id=".$u." AND mandanten_id=".$c." AND (status_benutzer=0 OR status_benutzer IS NULL) ORDER BY punkte DESC");
	$a=(int)db_value("SELECT b_admin FROM benutzer WHERE benutzer_id=".$u." AND personen_id=".$p." AND mandanten_id=".$c." AND (status_benutzer=0 OR status_benutzer IS NULL) ORDER BY punkte DESC");
//echo('$c='.$c.', $p='.$p.', $u='.$u.', $a='.$a.'<br />');

	// Get program class for user
	getPrgClass(false,$c,$u,$p,$a);

	// Get last db change date
	$d1=db_value("SELECT MAX(zeitstempel) FROM trans WHERE transakt_id>0");
	$d2=db_value("SELECT MAX(zeitstempel) FROM trans_aktionen WHERE trans_id>0");
	$d3=db_value("SELECT MAX(zeitstempel) FROM benutzergruppenzo WHERE benutzer_id=".$u." AND mandanten_id=".$c);
	$d4=db_value("SELECT MAX(zeitstempel) FROM rechte_trans WHERE mandanten_id=".$c);
	$md=max($d1, max($d2, max($d3,$d4)));
//echo('$d1='.$d1.', $d2='.$d2.', $d3='.$d3.', $d4='.$d4.' -- $md='.$md.'<br />');

	// Get file name for saved rights and file's last edit date
	$fn=$GLOBALS['usrpath'].'C'.substr('0000'.$c,-4).'/P'.substr('000000000'.$p,-9).'/apr.fpi';
	$ad='';
	if(file_exists($fn))
		$ad=date('YmdHis',filemtime($fn));
//echo('Load authorized program functions: $fn='.$fn.', $ad='.$ad.'<br />');

	// Load last saved rights file if save date is later than last db change date
	if(strlen($ad) && ($ad > $md))
	{
		require($fn);
	}
	// Create rights file and save it
	else
	{
		// Get all program function infos
		$sql  = "SELECT a.trans_id, a.aktionscode, a.defaultaktion, t.trans_art FROM trans_aktionen a, trans t";
		$sql .= " WHERE t.transakt_id=a.trans_id AND t.transakt_id>0";
		$sql .= " ORDER BY a.trans_id";
		$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);

		// Get rights for each program function
		foreach($tmp as $t)
		{
			$tid=(int)$t['trans_id'];
			$prgrights[$tid]=array('ta'=>(int)$t['trans_art'], 'da'=>(int)$t['defaultaktion'], 'ac'=>(int)$t['aktionscode'], 'tr'=>-1, 'disp'=>false);

			// Check rights except if already denied
			if($prgrights[$tid]['tr'] < 0)
			{
				$cr=check_rights($tid, $c,$u, $prgrights[$tid]['ta']);
				if(($tid != 158) && $cr)
				{
					// Sysadmin has all rights
					if(!$p || !$c)
						$prgrights[$tid]['tr']=(int)(1 | $prgrights[$tid]['ac']);
					else
					{
						// Get user groups for this user
						if(!isset($groups))
						{
							$groups=get_benutzergruppen($u,$c, true);
//fxDebug($groups,'$groups');
						}

						// No rights if in no user group
						if(!$groups)
							$prgrights[$tid]['tr']=0;
						// Assign rights according to user group
						else
						{
							// Get program rights according to user groups of this user
							if(!isset($rights))
							{
								$rights=array();

								$sql="SELECT benutzergr_id, trans_id, aktionscode FROM rechte_trans WHERE benutzergr_id IN (".$groups.") AND mandanten_id=".$c;
								$rtmp=db_values($sql,'*');
								if(fxIsArray($rtmp))
								{
									foreach($rtmp as $rt)
									{
										$rtid=(int)$rt['trans_id'];
										if(!isset($rights[$rtid]))
											$rights[$rtid]=0;
										$rights[$rtid] |= (int)$rt['aktionscode'];
									}
								}
//fxDebug($rights,'$rights');
							}

							$pr=0;
							if(isset($rights[$tid]))
							{
								$pr=$prgrights[$tid]['ac'];
								if(($prgrights[$tid]['ta'] != 1) && ($prgrights[$tid]['ta'] < 20))
									$pr &= $rights[$tid];
							}

							// If at least one right is assigned, then action=0 (2^0=1) is also set
							if($pr)
								$pr |= 1;

							$prgrights[$tid]['tr']=$pr;
						}
					}
				}
				else
					$prgrights[$tid]['tr']=(int)(1 | $prgrights[$tid]['ac']);

				// Display?
				// ...Sysadmin is omnipotent
				if(!$p)
					$prgrights[$tid]['disp']=true;
				else
				{
					switch($prgrights[$tid]['ta'])
					{
						case 40:	// ...Sysadmin
							if($GLOBALS['prgclass'] >= $prgrights[$tid]['ta'])
								$prgrights[$tid]['disp']=true;
						break;

						case 30:	// ...Admin1
							if(($all === 2) || (($c == 1) && ($all || ($GLOBALS['prgclass'] >= $prgrights[$tid]['ta']) || $prgrights[$tid]['tr'])))
								$prgrights[$tid]['disp']=true;
						break;

						case 27:	// ...Admin1-User
							if(($all === 2) || (($c == 1) && ($all || ($GLOBALS['prgclass'] >= 30) || $prgrights[$tid]['tr'])))
								$prgrights[$tid]['disp']=true;
						break;

						case 20:	// ...Admin
							if($all || ($GLOBALS['prgclass'] >= $prgrights[$tid]['ta']) || $prgrights[$tid]['tr'])
								$prgrights[$tid]['disp']=true;
						break;

						case 17:	// ...Admin-User-Program
							if($all || ($GLOBALS['prgclass'] >= 20) || $prgrights[$tid]['tr'])
								$prgrights[$tid]['disp']=true;
						break;

						case 16:	// ...FeRox-Program
 	 						if($all || (($GLOBALS['prgclass'] >= 10) && ($GLOBALS['prgclass'] < 40) && $prgrights[$tid]['tr']))
								$prgrights[$tid]['disp']=true;
						break;

						case 15:	// ...Test-Program
							$prgrights[$tid]['disp']=true;
						break;

						case 10:	// ...User-Program
	 						if($all || (($GLOBALS['prgclass'] >= 10) && ($GLOBALS['prgclass'] < 40) && $prgrights[$tid]['tr']))
								$prgrights[$tid]['disp']=true;
						break;

						case 1:		// ...Standard-Programme
							$prgrights[$tid]['disp']=true;
						break;
					}
				}
			}
		}

		// Save rights
		$txt="<"."?"."php".$GLOBALS['nl_b']."\$prgrights=array(".$GLOBALS['nl_b'];
		$cnt=1;
		$soa=sizeof($prgrights);
		foreach($prgrights as $tid => $pra)
		{
			$txt .= "\t".$tid." => array('ta'=>".$pra['ta'].", 'da'=>".$pra['da'].", 'ac'=>".$pra['ac'].", 'tr'=>".$pra['tr'].", 'disp'=>";
			if($pra['disp'])
				$txt .= "true";
			else
				$txt .= "false";
			$txt .= ")";
			if($cnt < $soa)
				$txt .= ",";
			$txt .= $GLOBALS['nl_b'];

			$cnt++;
		}
		$txt .= ");".$GLOBALS['nl_b']."?".">";
//echo('<pre>'.fxHtmlEncode($txt).'</pre>');

		fxSave($fn, $txt, true);
	}

	getPrgClass();
	return $prgrights;
}

/**
 * Exists program function?
 *
 * @param integer $trans - Mandatory parameter:  Program function number
 *
 * @return Program function number on success, else 0
 */
function tr_program($trans)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$trans=(int)$trans;
	if(!$trans)
		return 0;

	// Exists unique program?
	if((!isset($GLOBALS['prgdir'][$trans]) && !isset($GLOBALS['sppdir'][$trans])) || (isset($GLOBALS['prgdir'][$trans]) && isset($GLOBALS['sppdir'][$trans])))
		return 0;

	// Extended Block Transaction?
	if(isset($GLOBALS['sppdir'][$trans]) && (!function_exists('fxsppCheckEBTR') || !fxsppCheckEBTR($trans)))
		return 0;

	return $trans;
}

/**
 * #############################################################################
 * ##   tr_rechte                                                             ##
 * #############################################################################
 *
 * @param various $trans - Mandatory parameter:                  ???PARAMETER???
 * @param integer $u     - Optional parameter (default = 0):     ???PARAMETER???
 * @param boolean $ac    - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function tr_rechte($trans, $u=0, $ac=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$iright_atrans=false;	// Implicit rights for alias transaction?

	$trans=tr_program($trans);
	if(!$trans)
		return 0;

	$c=(int)$GLOBALS['fxpglobals']['client'];
	$u=(int)$u;
	if($u < 0)	// Get user id from person id
		$u=(int)db_value("SELECT benutzer_id FROM benutzer WHERE personen_id=".abs($u)." AND mandanten_id=".$c." AND (status_benutzer IS NULL OR status_benutzer=0 OR status_benutzer=1) ORDER BY punkte DESC");
	else if(!$u && isset($GLOBALS['fxpglobals']['user']))
		$u=(int)$GLOBALS['fxpglobals']['user'];

	if(!$c || !$u)
		return 0;

	// Get rights?
	if(!fxIsArray($GLOBALS['fxpvars']['trrights'][$trans]))
	{
		$GLOBALS['fxpvars']['trrights'][$trans]=array(
			'ta' => 0,
			'at' => 0,
			'da' => 0,
			'ac' => 0,
			'tr' => array($u=>-1)
		);

		$sql="SELECT a.aktionscode, a.defaultaktion, t.trans_art, t.alias_trans FROM trans_aktionen a, trans t WHERE t.transakt_id=a.trans_id AND a.trans_id=".$trans;
		$tr_array=db_values($sql);
		if(fxIsArray($tr_array))
		{
			$GLOBALS['fxpvars']['trrights'][$trans]['ta']=$tr_array['trans_art'];
			$GLOBALS['fxpvars']['trrights'][$trans]['at']=$tr_array['alias_trans'];
			$GLOBALS['fxpvars']['trrights'][$trans]['da']=$tr_array['defaultaktion'];
			$GLOBALS['fxpvars']['trrights'][$trans]['ac']=$tr_array['aktionscode'];
		}
	}
	if(fxIsArray($GLOBALS['fxpvars']['trrights'][$trans]) && !isset($GLOBALS['fxpvars']['trrights'][$trans]['tr'][$u]))
		$GLOBALS['fxpvars']['trrights'][$trans]['tr'][$u]=-1;
	if(fxIsArray($GLOBALS['fxpvars']['trrights'][$trans]) && ($GLOBALS['fxpvars']['trrights'][$trans]['tr'][$u] < 0))
	{
		$GLOBALS['fxpvars']['trrights'][$trans]['tr'][$u]=0;

		$cr=check_rights($trans, $c,$u);
		if(($trans != 158) && $cr)
		{
			// Rechte ermitteln
			if(!$GLOBALS['fxpglobals']['person'] || !$c)
				$GLOBALS['fxpvars']['trrights'][$trans]['tr'][$u]=(int)(1 | $GLOBALS['fxpvars']['trrights'][$trans]['ac']);
			else
			{
				// Benutzergruppen ermitteln
				$groups=get_benutzergruppen($u,$c, true);
				if(!$groups)
					return 0;
				else
				{
					$atrans=$trans;
					if($iright_atrans && $GLOBALS['fxpvars']['trrights'][$trans]['at'])
						$atrans .= ",".$GLOBALS['fxpvars']['trrights'][$trans]['at'];
					$sql  = "SELECT r.trans_id, r.aktionscode, r.benutzergr_id, t.trans_art";
					$sql .= " FROM trans t LEFT JOIN rechte_trans r ON r.trans_id=t.transakt_id";
					$sql .= " WHERE t.transakt_id IN (".$atrans.") AND ((r.mandanten_id=".$c." AND r.benutzergr_id IN (".$groups.")) OR t.trans_art=1)";
					$sql .= " ORDER BY r.aktionscode DESC";
					$tmp=db_values($sql,'*');
					if(!fxIsArray($tmp))
						return 0;
					else
					{
						// Recht ermitteln
						$recht=0;
						foreach($tmp as $item)
						{
							if(($GLOBALS['fxpvars']['trrights'][$trans]['ta'] == 1) || ($GLOBALS['fxpvars']['trrights'][$trans]['ta'] >= 20))
								$item['aktionscode']=$GLOBALS['fxpvars']['trrights'][$trans]['ac'];
							$item['aktionscode']=(int)$item['aktionscode'] & $GLOBALS['fxpvars']['trrights'][$trans]['ac'];
							$recht |= $item['aktionscode'];
						}

						// Falls mind. ein Recht vorhanden, wird aktion=0 (2^0=1) auch noch gesetzt.
						if($recht)
							$recht |= 1;

						$GLOBALS['fxpvars']['trrights'][$trans]['tr'][$u]=(int)$recht;
					}
				}
			}
		}
		else
			$GLOBALS['fxpvars']['trrights'][$trans]['tr'][$u]=(int)(1 | $GLOBALS['fxpvars']['trrights'][$trans]['ac']);
	}

	if($ac)
		return $GLOBALS['fxpvars']['trrights'][$trans]['tr'][$u];
	return $GLOBALS['fxpvars']['trrights'][$trans];
}

/**
 * #############################################################################
 * ##   check_rights                                                          ##
 * #############################################################################
 *
 * @param various $trans - Mandatory parameter:                ???PARAMETER???
 * @param integer $c     - Optional parameter (default = 0):   ???PARAMETER???
 * @param integer $u     - Optional parameter (default = 0):   ???PARAMETER???
 * @param various $ta    - Optional parameter (default = -1):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function check_rights($trans, $c=0,$u=0, $ta=-1)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$c=(int)$c;
	if(!$c && isset($GLOBALS['fxpglobals']['client']))
		$c=(int)$GLOBALS['fxpglobals']['client'];
	$u=(int)$u;
	if(!$u && isset($GLOBALS['fxpglobals']['user']))
		$u=(int)$GLOBALS['fxpglobals']['user'];
	if(!$c || !$u)
		return false;

	$id=$c.'_'.$u;

	// Benutzerklasse
	if(!isset($GLOBALS['uprgclassa']))
		$GLOBALS['uprgclassa']=array();
	if(!isset($GLOBALS['uprgclassa'][$id]))
	{
		$sql="SELECT personen_id, b_admin  FROM benutzer WHERE benutzer_id=".$u." AND mandanten_id=".$c;
		$uda=db_values($sql);
		$GLOBALS['uprgclassa'][$id]=array('client'=>$c, 'user'=>$u, 'person'=>(int)$uda['personen_id'], 'admin'=>(int)$uda['b_admin'], 'pclass'=>1);
		$GLOBALS['uprgclassa'][$id]['pclass']=getPrgClass(true, $GLOBALS['uprgclassa'][$id]['client'],$GLOBALS['uprgclassa'][$id]['user'],$GLOBALS['uprgclassa'][$id]['person'],$GLOBALS['uprgclassa'][$id]['admin']);
	}

	// Transaktionsart
	if($ta < 0)
	{
		if(fxIsArray($GLOBALS['fxpvars']['trrights'][$trans]))
			$ta=$GLOBALS['fxpvars']['trrights'][$trans]['ta'];
		else
			$ta=db_value("SELECT trans_art FROM trans WHERE transakt_id=".$trans);
	}

	$check_rights=true;
	// Sysadmin darf alles!
	if($GLOBALS['uprgclassa'][$id]['pclass'] >= 40)
		$check_rights=false;
	// Standard- und Testprogramme immer erlauben
	else if(($ta == 1) || ($ta == 15))
		$check_rights=false;
	// Admin(1) darf alle Adminprogramme
	else if($GLOBALS['uprgclassa'][$id]['pclass'] == 30)
	{
		if(($ta >= 17) && ($ta != 27) && ($ta < 40))
			$check_rights=false;
	}
	// Admin darf Adminprogramme, ausser für Mandant 1
	else if($GLOBALS['uprgclassa'][$id]['pclass'] == 20)
	{
		if(($ta >= 17) && ($ta <= 20))
			$check_rights=false;
	}

//echo('uprgclass='.$GLOBALS['uprgclassa'][$id]['pclass'].', trans_art['.$trans.']='.$ta.' &gt; <b>$check_rights='.$check_rights.'</b><br />'.$GLOBALS['nl']);
	return $check_rights;
}

/**
 * #############################################################################
 * ##   darf_kosten_sehen                                                     ##
 * #############################################################################
 *
 * @param integer $prid - Optional parameter (default = 0): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function darf_kosten_sehen($prid=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Sysadmin darf immer alle Kosten sehen
	if($GLOBALS['prgclass'] >= 40)
		return 3;

	// Ausnahmetransaktionen, bei denen man immer alle Kosten sehen kann/muss, z.B. wegen Eingabe eigener Daten
	if(isset($GLOBALS['fxpglobals']['tr']) && $GLOBALS['fxpglobals']['tr'])
	{
		// ...81:	ZE (Tag)
		// ...83:	ZE (Woche/Monat)
		// ...34:	ZE (Summe)
		// ...150:	Reisekostenerfassung, da sonst Belegseingabefelder leer
		$except_tr=array(81=>true, 83=>true, 34=>true, 150=>true);

		if($except_tr[$GLOBALS['fxpglobals']['tr']])
			return 3;
	}

	// Normale Prüfroutine
	$prid=(int)$prid;

	// Kostenrecht des Benutzers ermitteln
	$sql="SELECT kosten FROM benutzer WHERE benutzer_id=".$GLOBALS['fxpglobals']['user']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$kst=(int)db_value($sql);

	switch($kst)
	{
		case 2161:	// Benutzer darf KEINE Kosten sehen
			return 0;

		case 2903:	// Benutzer darf NUR INTERNE Kosten sehen
			return 1;

		case 2904:	// Benutzer darf NUR EXTERNE Kosten sehen
			return 2;

		case 2162:	// Benutzer darf ALLE Kosten sehen
			return 3;

		default:	// 2160=Benutzer darf Kosten sehen entsprechend seiner MITARBEITER-ROLLE
			// ...Geschäftsführer [2] oder Budgetgenehmigung [-4]
			if(isset($GLOBALS['fxpglobals']['persdat']['roles'][2]) || isset($GLOBALS['fxpglobals']['persdat']['roles'][-4]))
				return 3;

			// ...Projektleitung [1] oder Projektmanagement [-2]
			else if(isset($GLOBALS['fxpglobals']['persdat']['roles'][1]) || isset($GLOBALS['fxpglobals']['persdat']['roles'][-2]))
			{
				// Prüfung für ein spezielles Projekt durchführen?
				if($prid)
				{
					// Ja, dann prüfen ob eingeloggte Person Projektmanager, Projektleiter oder Stellvertreter ist
					$sql  = "SELECT projekt_id FROM projekte WHERE projekt_id=".$prid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$sql .= " AND (projektmanager=".$GLOBALS['fxpglobals']['person']." OR projektleiter=".$GLOBALS['fxpglobals']['person']." OR vertreter=".$GLOBALS['fxpglobals']['person'].")";
					$pml =(int)db_value($sql);
					// Ja, dann ALLE Kosten
					if($pml)
						return 3;
					// Nein, dann KEINE Kosten
					return 0;
				}

				// Nein, dann ALLE Kosten
				return 3;
			}

			// ...Ansprechpartner Auftraggeber
			else if($GLOBALS['fxpglobals']['persdat']['personentyp'] == 620)
			{
				// Prüfung für ein spezielles Projekt durchführen?
				if($prid)
				{
					// Ja, dann prüfen ob eingeloggte Person Ansprechpartner des Auftraggebers ist
					$sql="SELECT projekt_id FROM projekte WHERE projekt_id=".$prid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND ansprechpartner=".$GLOBALS['fxpglobals']['person'];
					$ans=(int)db_value($sql);
					// Ja, dann EXTERNE Kosten
					if($ans)
						return 2;
					// Nein, dann KEINE Kosten
					return 0;
				}
				// Nein, dann KEINE Kosten
				return 0;
			}

			// ...KEINE Kosten
			return 0;
	}
}

/**
 * Determines the email send method (SMTP or MAIL) according to the SMTP host setting
 *
 * @param string $smtp_host - Mandatory parameter:  SMTP host string, usually the setting from the ini file, which is parsed into the variable $GLOBALS['fxpglobals']['dbparam']['smtp_host']
 *
 * @return String 'smtp' if a SMTP Host is set, else 'mail'
 */
function getSendingMethod($smtp_host)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(strlen(trim($smtp_host)) > 0)
		return 'smtp';
	else
		return 'mail';
}

/**
 * erstellt aus $GLOBALS['fxpglobals']['dbparam']['smtp_*'] ein Array für den SMTP-Zugang
 * @return array das Array für den SMTP-Zugang mit Indizes 'port', 'helo', 'email', 'account' und 'password'
 */
function getSMTPArray()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$smtpArray=array();
	$smtpArray['port']=(int)$GLOBALS['fxpglobals']['dbparam']['smtp_port'];
	if(!$smtpArray['port'])
		$smtpArray['port']=25;

	$smtpArray['helo']=trim($GLOBALS['fxpglobals']['dbparam']['smtp_helo']);
	if(!strlen($smtpArray['helo']))
		$smtpArray['helo']=NULL;

	$smtpArray['email']=trim($GLOBALS['fxdb']['smtp_email']);
	if(!strlen($smtpArray['email']))
		$smtpArray['email']=NULL;

	$smtpArray['account']=trim($GLOBALS['fxdb']['smtp_account']);
	if(!strlen($smtpArray['account']))
		$smtpArray['account']=NULL;

	$smtpArray['password']=trim($GLOBALS['fxdb']['smtp_password']);
	if(!strlen($smtpArray['password']))
		$smtpArray['password']=NULL;
	return $smtpArray;
}

/**
 * liefert zum aktuellen Wochentag den Wochentag als String
 * @param int $day, der Wochentag als INT, mit 0=Sonntag
 * @return string der zum Wochentag gehörige String (in deutsch), mit 0 => 'Sonntag', etc.
 */
function dayToColumnName($day)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$day--;
	if($day == -1)
		$day=6;
	 return db_value("SELECT tabwert FROM wertetab WHERE wertetabid=".(685 + $day)." AND id_sprache=1");
}

/**
 * escaped einen String zum Schreiben in die DB
 */
function escapeANSI($string)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return str_replace("'", "''", $string);
}

/**
 * ???FUNCTION???
 *
 * @param various $atype     - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $ret_class - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getAbsentClass($atype,$ret_class=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($atype == FXP_AT_ABSENT)
	{
		$appc='eeee99';
		$cls='lightred';
	}
	else if(($atype == FXP_AT_VACATIONREQUEST) || ($atype == FXP_AT_VACATION))
	{
		$appc='88ff88';
		$cls='green';
	}
	else if(($atype == FXP_AT_SPECIALVACATIONREQUEST) || ($atype == FXP_AT_SPECIALVACATION))
	{
		$appc='88ffff';
		$cls='lightgreen';
	}
	else if(($atype == FXP_AT_TRAININGREQUEST) || ($atype == FXP_AT_TRAINING))
	{
		$appc='8888ff';
		$cls='lightblue';
	}
	else if(($atype == FXP_AT_SICKCALL) || ($atype == FXP_AT_SICK))
	{
		$appc='ff8888';
		$cls='red';
	}
	else
	{
		$appc='aa88aa';
		$cls='darkgrey';
	}

	if($ret_class)
		return $cls;
	return array('cls'=>$cls, 'appc'=>$appc);
}

/**
 * ???FUNCTION???
 *
 * @param various $title - Mandatory parameter:               ???PARAMETER???
 * @param integer $lng   - Optional parameter (default = 0):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function createTRHelpTitlepage($title,$lng=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$lng=(int)$lng;
	if(!$lng)
		$lng=$GLOBALS['fxpglobals']['lang'];

	$ftitle='';
	for($i=0; $i<strlen($title); $i++)
		$ftitle .= strtoupper(substr($title,$i,1)).' ';
	$ftitle=trim($ftitle);

	$mpr=array('./'=>'../../');

	$html  = '<table width="100%" border="0" cellpadding="0" cellspacing="0">'.$GLOBALS['nl'];
	$html .= '	<tr>'.$GLOBALS['nl'];
	$html .= '		<td>&nbsp;</td>'.$GLOBALS['nl'];
	$html .= '		<td align="left" valign="top"><br /><img src="'.strtr($GLOBALS['gfxpath'],$mpr).'wwwfxp.png"></td>'.$GLOBALS['nl'];
	$html .= '		<td align="right" valign="top">'.$GLOBALS['nl'];
	$html .= '			<table border="0" cellpadding="0" cellspacing="0">'.$GLOBALS['nl'];
	$html .= '				<tr><td valign="top"><br /><br /><b style="font-size:14pt;">'.$ftitle.'</b>&nbsp;&nbsp;<br /><br /><br /><br /><br /><br /><br /></td></tr>'.$GLOBALS['nl'];
	$html .= '				<tr><td align="center"><img src="'.strtr($GLOBALS['gfxpath'],$mpr).'fxptg_'.$lng.'.png"></td></tr>'.$GLOBALS['nl'];
	$html .= '				<tr><td align="right" valign="bottom"><br /><br /><br /><br /><br /><br /><br /><br /><b style="font-size:11pt;">VERSION&nbsp;'.FXP_VERSION.'</b>&nbsp;&nbsp;</td></tr>'.$GLOBALS['nl'];
	$html .= '			</table>'.$GLOBALS['nl'];
	$html .= '		</td>'.$GLOBALS['nl'];
	$html .= '		<td>&nbsp;</td>'.$GLOBALS['nl'];
	$html .= '	</tr>'.$GLOBALS['nl'];
	$html .= '</table>'.$GLOBALS['nl'];

	return $html;
}

/**
 * ???FUNCTION???
 *
 * @param various $title - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function createTRHelpHeadFoot($title)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(strlen($title))
	{
		if(!isset($GLOBALS['docwidth']))
			$GLOBALS['docwidth']=640;

		$mpr=array('./'=>'../../');

		$html ='<!DOCTYPE html>'.$GLOBALS['nl'];
		$html .= '<html>'.$GLOBALS['nl'];
		$html .= '	<head>'.$GLOBALS['nl'];
		$html .= '		<!-- © FeRox Management Consulting GmbH & Co. KG -->'.$GLOBALS['nl'];
		$html .= '		<title>fx-project: '.$title.' (V'.FXP_VERSION.')</title>'.$GLOBALS['nl'].$GLOBALS['nl'];
		$html .= '		<meta charset="'.FXP_CHARSET.'">'.$GLOBALS['nl'];
		$html .= '		<meta name="viewport" content="width=device-width, initial-scale=1">'.$GLOBALS['nl'];
		$html .= '		<meta name="author" content="FeRox Management Consulting GmbH & Co. KG">'.$GLOBALS['nl'];
		$html .= '		<meta name="format-detection" content="telephone=no">'.$GLOBALS['nl'];
		$html .= '		<meta name="date" content="'.substr($GLOBALS['datetime'],0,4).'-'.substr($GLOBALS['datetime'],4,2).'-'.substr($GLOBALS['datetime'],6,2).'T00:00:00+00:00">'.$GLOBALS['nl'];
		$html .= '		<meta name="robots" content="no follow">'.$GLOBALS['nl'].$GLOBALS['nl'];
		$html .= '		<!-- CSS -->'.$GLOBALS['nl'];
		$html .= '		<link rel="stylesheet" href="'.strtr($GLOBALS['cssfile'],$mpr).'" type="text/css">'.$GLOBALS['nl'].$GLOBALS['nl'];
		$html .= '		<!-- FAVICON -->'.$GLOBALS['nl'];
		$html .= '		<link rel="shortcut icon" href="'.fxGetFavicon().'">'.$GLOBALS['nl'];
		$html .= '	</head>'.$GLOBALS['nl'];
		$html .= '	<body style="background:#ffffff;margin:8px;padding:0;font-size:11.0pt;font-family:calibri,sans-serif;">'.$GLOBALS['nl'];
		$html .= '		<center>'.$GLOBALS['nl'];
		$html .= '			<div style="font-size:11.0pt;font-family:calibri,sans-serif;width:'.($GLOBALS['docwidth']+4).'px;text-align:left;white-space:normal;">'.$GLOBALS['nl'];
	}
	else
	{
		$html .= '			</div>'.$GLOBALS['nl'];
		$html .= '		</center>'.$GLOBALS['nl'];
		$html .= '	</body>'.$GLOBALS['nl'];
		$html .= '</html>';
	}

	return $html;
}

/**
 * ???FUNCTION???
 *
 * @param various $trans - Mandatory parameter:                   ???PARAMETER???
 * @param integer $lng   - Optional parameter (default = 0):      ???PARAMETER???
 * @param integer $slng  - Optional parameter (default = 0):      ???PARAMETER???
 * @param boolean $rhtml - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function createTRHelpFile($trans,$lng=0,$slng=0,$rhtml=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Common variables
	if(!isset($GLOBALS['maxwidth_screenshot']))
		$GLOBALS['maxwidth_screenshot']=664;
	if(!isset($GLOBALS['docwidth']))
		$GLOBALS['docwidth']=640;
	// Language
	$blng=$GLOBALS['fxpglobals']['lang'];
	$lng=(int)$lng;
	if(!$lng)
		$lng=$GLOBALS['fxpglobals']['lang'];
	$GLOBALS['fxpglobals']['lang']=$lng;
	$slng=(int)$slng;
	if($slng)
		$GLOBALS['intern']=$slng;

	$mr='';
	$mp='';
	$mn=loadMenu(true);
	foreach($mn as $mnr => $mi)
	{
		if(!$mi['tr'] || ($mi['tr'] != $trans))
			continue;
		$mr=$mnr;
		$mbnr=substr($mnr,0,-3);
		while(strlen($mbnr) > 1)
		{
			$tx=$mn[$mbnr]['tx'];
			if($slng)
				$tx='<span class="manual_h1 dint" title="TX (menu file): '.$mbnr.'">'.$tx.'</span>';
			$mp=$tx.' &gt; '.$mp;
			$mbnr=substr($mbnr,0,-3);
		}
		break;
	}

	$str=substr('00000'.(int)$trans,-6);

	// Create help
	$html='';
	if(!$slng && !$rhtml)
	{
		// Head
		if($GLOBALS['fxpglobals']['lang'] == 1)
			$title='Programmfunktionshilfe #'.$str;
		else
			$title='Program Function Help #'.$str;

		$html .= createTRHelpHeadFoot($title);
	}
	else
		$html .= '		<div style="font-size:11.0pt;font-family:calibri,sans-serif;width:'.($GLOBALS['docwidth']+4).'px;text-align:left;">'.$GLOBALS['nl'];
	// Transaction description
	$tbr=array();
	$sql="SELECT text_kurz, text_varlang, zeitstempel FROM texte WHERE id_referenz=".$trans." AND id_ref_art=6 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND text_varlang IS NOT NULL";
	$tmp=db_values($sql);
	if(fxIsArray($tmp) && sizeof($tmp))
		$tbr=array('title'=>$tmp['text_kurz'], 'text'=>$tmp['text_varlang'], 'ts'=>$tmp['zeitstempel']);
	if(strlen($mr))
	{
		// ...Breadcrumb (from menu file)
		$html .= '		<a name="'.$str.'"><h1 class="manual_h1">'.$mp;
		if($slng)
			$html .= '<span class="manual_h1 dint" title="TX (menu file): '.$mr.'">';
		$html .= $mn[$mr]['tx'];
		if($slng)
			$html .= '</span>';
		$html .= '</h1></p></a><p></p>'.$GLOBALS['nl'];	// </h1> wird ersetzt durch </h1><p>

		// ...Transaction short description (from menu file)
		$html .= '<p class="manual_bld" style="color:#888888;">';
		if($slng)
			$html .= '<span class="manual_bld dint" title="TT (menu file): '.$mr.'">';
		$html .= $mn[$mr]['tt'];
		if($slng)
			$html .= '</span>';
		$html .= '</p>'.$GLOBALS['nl'];
	}
	else if(sizeof($tbr) && strlen($tbr['title']))
	{
		$html .= '		<a name="'.$str.'"><h1 class="manual_h1">';
		if($slng)
			$html .= '<div class="dintl" title="[text_kurz] in table &quot;texte&quot;: id_referenz='.$trans.', id_ref_art=6, id_sprache='.$GLOBALS['fxpglobals']['lang'].'" onclick="window.open(\'language.php?mfield=text_kurz&slng='.$slng.'&dlng='.$GLOBALS['fxpglobals']['lang'].'&pks_id_referenz='.$trans.'&pks_id_ref_art=6'.fxReturnSessionParams().'\');">';
		$html .= $tbr['title'];
		if($slng)
			$html .= '</div>';
		$html .= '</h1></p></a><p></p>'.$GLOBALS['nl'];	// </h1> wird ersetzt dur </h1><p>
	}
	else
		$html .= '		<a name="'.$str.'">&nbsp;</a>'.$GLOBALS['nl'];

	// ...Transaction description (from table "texte")
	if(sizeof($tbr))
	{
		if(strlen($tbr['text']))
		{
			if(strlen($mr))
				$html .= '<br />'.$GLOBALS['nl'];
			if($slng)
				$html .= '<div class="dintl" title="[text_varlang] in table &quot;texte&quot;: id_referenz='.$trans.', id_ref_art=6, id_sprache='.$GLOBALS['fxpglobals']['lang'].'" onclick="window.open(\'language.php?mfield=text_varlang&slng='.$slng.'&dlng='.$GLOBALS['fxpglobals']['lang'].'&pks_id_referenz='.$trans.'&pks_id_ref_art=6'.fxReturnSessionParams().'\');">';
			$html .= '	<p class="manual_std">'.extract_meldung($tbr['text'], true, $trans, $GLOBALS['maxwidth_screenshot']).'</p>'.$GLOBALS['nl'];
			if($slng)
				$html .= '</div>';
		}
		else if($slng)
			$html .= '<div class="dintle" onclick="window.open(\'language.php?mfield=text_varlang&slng='.$slng.'&dlng='.$GLOBALS['fxpglobals']['lang'].'&pks_id_referenz='.$trans.'&pks_id_ref_art=6'.fxReturnSessionParams().'\');">&nbsp;[text_varlang] in table &quot;texte&quot; empty: id_referenz='.$trans.', id_ref_art=6, id_sprache='.$GLOBALS['fxpglobals']['lang'].'&nbsp;</div>';
	}
	else if($slng)
		$html .= '	<div class="dintle" onclick="window.open(\'language.php?mfield=text_varlang&slng='.$slng.'&dlng='.$GLOBALS['fxpglobals']['lang'].'&pks_id_referenz='.$trans.'&pks_id_ref_art=6'.fxReturnSessionParams().'\');">&nbsp;Transaction description in &quot;table&quot; texte missing: id_referenz='.$trans.', id_ref_art=6, id_sprache='.$GLOBALS['fxpglobals']['lang'].'&nbsp;</div>';

	// ...Transaction pic (png or jpg)
	$mpt=$GLOBALS['docpath'].substr('00'.$GLOBALS['fxpglobals']['lang'],-2).'/';
	$mgr='SCR/'.$str.'_000000.png';
	if(!file_exists($mpt.$mgr))
		$mgr=substr($mgr,0,-3).'jpg';
	if(file_exists($mpt.$mgr))
	{
		$html .= '<br />'.$GLOBALS['nl'];
		if($slng)
		{
			$ims=GetImageSize($mpt.$mgr);
			$html .= '<div class="dint" title="Transaction screenshot: &quot;'.$mpt.$mgr.'&quot; ('.$ims[0].'x'.$ims[1].')">';
		}
		$html .= '<p class="manual_std">'.dok_screenshot($mpt, $mgr, $GLOBALS['maxwidth_screenshot']).'</p>'.$GLOBALS['nl'];
		if($slng)
			$html .= '</div>';
	}
	$html .= ' '.$GLOBALS['nl'];

	// ...Masks
	$sql="SELECT nummer FROM trans_maske WHERE transakt_id=".$trans." AND nummer>0 ORDER BY zeile";
	$msa=db_values($sql,'*');
	if(fxIsArray($msa))
	{
		$mncsv='';
		foreach($msa as $c => $m)
		{
			if($c)
				$mncsv .= ',';
			$mncsv .= (int)$m['nummer'];
		}

		// ...Get field descriptions
		$fbr=array();
		$sql="SELECT id_referenz, id_sprache, text_kurz, text_varlang, anmerkungintern, literal_5, literal_10, literal_15, literal_20, literal_text, zeitstempel FROM texte WHERE id_referenz>0 AND id_ref_art=3 AND id_sprache=".$GLOBALS['fxpglobals']['lang'];
		$tmp=db_values($sql,'*');
		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
				$fbr[$t['id_referenz']]=array('text_kurz'=>$t['text_kurz'], 'text_varlang'=>$t['text_varlang'], 'anmerkung_intern'=>$t['anmerkung_intern'], 'literal_5'=>$t['literal_5'], 'literal_10'=>$t['literal_10'], 'literal_15'=>$t['literal_15'], 'literal_20'=>$t['literal_20'], 'literal_text'=>$t['literal_text'], 'ts'=>$t['zeitstempel']);
		}

		// ...Get mask fields
		$mfl=array();
		$sql  = "SELECT m.nummer, m.maskenart, mf.nummer, mf.zeile, mf.spalte, mf.anzeigeart, mf.eingabefeld, mf.anzeigegroesse1, mf.anzeigegroesse2, mf.spaltenanzahl, mf.literal_overwrite, f.feldname, f.alias_zu, f.feldtyp";
		$sql .= " FROM masken m, maskenfelder mf, felder f";
		$sql .= " WHERE mf.nummer=m.nummer AND f.id=mf.eingabefeld AND f.mandanten_id=0 AND m.nummer IN (".$mncsv.") AND mf.eingabefeld>0 AND mf.eingabefeld NOT IN (26,396,397) AND mf.anzeigeart>0 AND mf.anzeigeart NOT IN (10,13,18,19)";	// 10:Hidden, 13:Reset, 18:Literal_Frame + 19:Literal_Header ignorieren
		$sql .= " ORDER BY m.nummer,mf.zeile, mf.spalte";
		$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
		if(fxIsArray($tmp))
		{
			// Literal size
			$lit=array();
			$litc=array();
			foreach($tmp as $t)
			{
				$id=(int)$t['eingabefeld'];	// Field ID
				$mnr=(int)$t['nummer'];		// Mask number
				$typ=(int)$t['maskenart'];	// Mask type: 1=Single mask, 2=Single page list, 3=Total list, 4=Single page report, 5=Total report
				$fld=$t['feldname'];		// Field name
				$art=(int)$t['anzeigeart'];	// Field type: 1=Literal_5, ...

				$lg='';
				if(($art < 5) || ($art == 12) || ($art == 14) || ($art == 20))	// 1:Literal_5 - 4:Literal_20 or 12:Submit_Button or 14:Sum or 20:Literal_Text
				{
					$lg=getTRHelpLiteral($t);
					if(strlen($lg))
					{
						if(!fxIsArray($lit[$mnr]))
							$lit[$mnr]=array();
						$lit[$mnr][$fld]=$lg;
					}
				}

//				if(strlen($lg) && ((($typ == 2) || ($typ == 3)) && ($t['zeile'] == 1)))
				if(($typ > 1) && ($t['zeile'] == 1))
				{
					if(!strlen($lg))
						$lg=getTRHelpLiteral($t);
					if(strlen($lg))
					{
						if(!fxIsArray($litc[$mnr]))
							$litc[$mnr]=array();
						$litc[$mnr][$t['spalte']]=$id.'|'.$lg;
					}
				}
			}
//fxDebug($lit,'$lit');
//fxDebug($litc,'$litc');

			// Put mask fields in array
			foreach($tmp as $t)
			{
				$id=(int)$t['eingabefeld'];	// Field ID
				$mnr=(int)$t['nummer'];		// Mask number
				$typ=(int)$t['maskenart'];	// Mask type: 1=Single mask, 2=Single page list, 3=Total list, 4=Single page report, 5=Total report
				$fld=$t['feldname'];		// Field name
				$art=(int)$t['anzeigeart'];	// Field type: 1=Literal_5, ...

				if(($typ > 3) || (($art > 4) && ($art < 18) && ($art != 14)))	// Literalfelder nicht berücksichtigen, außer bei Reports
				{
					$lg='';
					if(is_array($litc[$mnr]) && ($t['zeile'] > 1))
					{
						if(isset($litc[$mnr][$t['spalte']]))
							$lg=$litc[$mnr][$t['spalte']];
					}
					else if(isset($lit[$mnr][$fld]))
						$lg=$lit[$mnr][$fld];
					else
						$lg=getTRHelpLiteral($t);
					if(!strlen($lg))
						$lg='#literal_15';
					if(!fxIsArray($mfl[$mnr]))
						$mfl[$mnr]=array();
					$mfl[$mnr][$id]=array('name'=>$fld, 'art'=>$art, 'typ'=>$t['feldtyp'], 'alias'=>$t['alias_zu'], 'lg'=>$lg, 'hide'=>false);
				}
			}
		}
//fxDebug($mfl,'$mfl');

		// ...Get hidden fields
		$harr=array();
		$hpath=$GLOBALS['docpath'].'hide.csv';
		if(file_exists($hpath))
		{
			$tarr=file($hpath);
			if(fxIsArray($tarr))
			{
				foreach($tarr as $tl => $tcsv)
				{
					if($tl > 0)
					{
						$tca=explode(';',$tcsv);
						$ttr=(int)$tca[0];	// Transaction number
						$tmn=(int)$tca[1];	// Mask number
						$tfn=(int)$tca[3];	// Field ID
						if(!fxIsArray($harr[$ttr]))
							$harr[$ttr]=array();
						if(!fxIsArray($harr[$ttr][$tmn]))
							$harr[$ttr][$tmn]=array();
						$harr[$ttr][$tmn][$tfn]=$tca[4];
					}
				}
			}
		}
//fxDebug($harr,'$harr');

		foreach($msa as $m)
		{
			$msnr=(int)$m['nummer'];
			$smnr=substr('000000'.$msnr,-6);

			// Mask title and description
			$mbr=array();
			$sql="SELECT t.id_referenz, t.id_sprache, t.text_kurz, t.text_varlang, t.zeitstempel, m.maskenart, m.name_maske, m.listenansicht FROM texte t, masken m WHERE m.nummer=t.id_referenz AND m.nummer=".$msnr." AND m.mandanten_id=0 AND t.id_referenz>0 AND t.id_ref_art=2 AND t.id_sprache=".$GLOBALS['fxpglobals']['lang'];
			$tmp=db_values($sql);
			if(fxIsArray($tmp))
				$mbr=array('title'=>$tmp['text_kurz'], 'text'=>$tmp['text_varlang'], 'name'=>$tmp['name_maske'], 'type'=>$tmp['maskenart'], 'list'=>$tmp['listenansicht'], 'ts'=>$tmp['zeitstempel']);
//fxDebug($mbr,'$mbr: '.$msnr);

			// ...Mask title
			if(isset($mbr['title']) && strlen($mbr['title']))
			{
				$mtitle=trim($mbr['title']);
				if($slng)
					$mtitle='<span class="manual_h2 dintl" title="Mask &quot;'.$mbr['name'].'&quot; ('.$msnr.') - [text_kurz] in table &quot;texte&quot;: id_referenz='.$msnr.', id_ref_art=2, id_sprache='.$GLOBALS['fxpglobals']['lang'].'" onclick="window.open(\'language.php?mfield=text_kurz&slng='.$slng.'&dlng='.$GLOBALS['fxpglobals']['lang'].'&pks_id_referenz='.$msnr.'&pks_id_ref_art=2'.fxReturnSessionParams().'\');">'.$mtitle.'</span>';
			}
			else
			{
				$mtitle='M'.$smnr;
				if($slng)
					$mtitle='<span class="manual_h2 dintle" title="Mask '.$msnr.' - [text_kurz] in table &quot;texte&quot; empty or missing: id_referenz='.$msnr.', id_ref_art=2, id_sprache='.$GLOBALS['fxpglobals']['lang'].'" onclick="window.open(\'language.php?mfield=text_kurz&slng='.$slng.'&dlng='.$GLOBALS['fxpglobals']['lang'].'&pks_id_referenz='.$msnr.'&pks_id_ref_art=2'.fxReturnSessionParams().'\');">'.$mtitle.'</span>';
			}
			$html .= '<br /><br /><br />'.$GLOBALS['nl'];
			$html .= '	<a name="'.$str.'_'.$smnr.'"><h2 class="manual_h2">'.$mtitle.'</h2></a>'.$GLOBALS['nl'];
			$html .= '  '.$GLOBALS['nl'];
			// ...Mask description
			if(isset($mbr['text']) && strlen($mbr['text']))
			{
				$html .= '<br />'.$GLOBALS['nl'];
				if($slng)
					$html .= '<div class="dintl" title="Mask &quot;'.$mbr['name'].'&quot; ('.$msnr.') - [text_varlang] in table &quot;texte&quot;: id_referenz='.$msnr.', id_ref_art=2, id_sprache='.$GLOBALS['fxpglobals']['lang'].'" onclick="window.open(\'language.php?mfield=text_varlang&slng='.$slng.'&dlng='.$GLOBALS['fxpglobals']['lang'].'&pks_id_referenz='.$msnr.'&pks_id_ref_art=2'.fxReturnSessionParams().'\');">';
				$html .= '	<p class="manual_std">'.extract_help($mbr['text'], $trans, $msnr).'</p>'.$GLOBALS['nl'];
				if($slng)
					$html .= '</div>';
			}
			else if($slng)
				$html .= '<div class="dintle" onclick="window.open(\'language.php?mfield=text_varlang&slng='.$slng.'&dlng='.$GLOBALS['fxpglobals']['lang'].'&pks_id_referenz='.$msnr.'&pks_id_ref_art=2'.fxReturnSessionParams().'\');">&nbsp;Mask '.$msnr.' - [text_varlang] in table &quot;texte&quot; empty: id_referenz='.$msnr.', id_ref_art=2, id_sprache='.$GLOBALS['fxpglobals']['lang'].'&nbsp;</div>';

			// ...Mask pic (png or jpg)
			$mgr='SCR/'.$str.'_'.$smnr.'.png';
			if(!file_exists($mpt.$mgr))
				$mgr=substr($mgr,0,-3).'jpg';
			if(file_exists($mpt.$mgr))
			{
				$html .= '<br />'.$GLOBALS['nl'];
				if($slng)
				{
					$ims=GetImageSize($mpt.$mgr);
					$html .= '<div class="dint" title="Mask screenshot: &quot;'.$mpt.$mgr.'&quot; ('.$ims[0].'x'.$ims[1].')">';
				}
				$html .= '<p class="manual_std">'.dok_screenshot($mpt, $mgr, $GLOBALS['maxwidth_screenshot']).'</p>'.$GLOBALS['nl'];
				if($slng)
					$html .= '</div>';
			}
			else if($slng)
			{
				$html .= '<br />'.$GLOBALS['nl'];
				$html .= '	<div class="dinte">&nbsp;Mask screenshot missing: &quot;'.$mpt.$mgr.'&quot;&nbsp;</div>';
			}

			// ..Hide fields?
			$som=0;
			if(fxIsArray($mfl[$msnr]))
				$som=sizeof($mfl[$msnr]);
			if($som && sizeof($harr))
			{
				foreach($mfl[$msnr] as $fld => $fi)
				{
					if(fxIsArray($harr[$trans]) && fxIsArray($harr[$trans][$msnr]) && isset($harr[$trans][$msnr][$fld]))
					{
						$mfl[$msnr][$fld]['hide']=true;
						$som--;
					}
				}
			}

			// Mask fields
			if($som || ($slng && $mfl[$msnr] && sizeof($mfl[$msnr])))
			{
				// ...Headline
				$GLOBALS['headline_set']=true;
				$html .= '<br />'.$GLOBALS['nl'];
				$html .= '		<p class="manual_std" style="position:relative;"><b class="manual_bold s4">'.meldung(2089,true).'</b></p>'.$GLOBALS['nl'];

				$button_set=false;
				foreach($mfl[$msnr] as $fld => $fi)
				{
					// ...Field literal
					$lga=explode('|',$fi['lg']);
					if(sizeof($lga) > 1)
					{
						$afn=(int)$lga[0];
						$lg=$lga[1];
					}
					else
					{
						$afn=$fld;
						$lg=$lga[0];
						if($fi['alias'] && (!isset($fbr[$afn]) || !strlen($fbr[$afn][$lg])))
							$afn=$fi['alias'];
					}
					$br=false;
					$ii_afn=$afn;
					$ii_atp='text_varlang';
					$flit=extract_lit($fbr[$afn]['text_varlang'], $trans, $msnr);
					if(!strlen($flit) && (substr($lg,0,1) == '*'))
					{
						$flit=substr($lg,1);
						$ii_atp='literal_overwrite';
					}
					if(!strlen($flit) && (substr($lg,0,1) == '#'))
					{
						$lg=substr($lg,1);
						$br=true;
					}
					if(!strlen($flit))
					{
						$flit=$fbr[$afn][$lg];
						$ii_atp=$lg;
					}
					if(!strlen($flit))
					{
						$flit=$fld;
						$ii_atp='';
						$br=true;
					}
					if($br)
						$flit='<i>'.$flit.'</i>';
					$hv=$trans.';'.$msnr.';;'.$ii_afn.';'.$fi['name'];	// 78;102;;1667;Dummy0

					if(!$fi['hide'])
					{
						$hi='';
						if($slng)
							$hi='<img class="dintl" src="'.$GLOBALS['gfxpath'].'fl_cl.png" style="position:absolute;left:700px;padding:2px;cursor:pointer;" title="[Hide] {hw} ('.$hv.')" onclick="var hh=document.getElementById(\'hide\'); if(hh) {hh.value=\'[H]'.$hv.'\';} var rs=document.getElementById(\'resubmit\'); if(rs) {rs.click();}">';

						if($fi['art'] == 12)	// Button
						{
							if(!$button_set)
								$html .= '<br />'.$GLOBALS['nl'];
							$cadd='';
							$tadd='';
							if($slng)
							{
								$cadd=' class="dintl';
								$tadd=' title="Field &quot;'.$fi['name'].'&quot; ('.$ii_afn.') - Literal ';
								if(strlen($ii_atp))
									$tadd .= '['.$ii_atp.']';
								else
								{
									$cadd .= 'e';
									$tadd .= 'empty';
								}
								$cadd .= '"';
								$tadd .= ': id_referenz='.$ii_afn.', id_refart=3, id_sprache='.$GLOBALS['fxpglobals']['lang'].'" onclick="window.open(\'language.php?mfield='.$ii_atp.'&slng='.$slng.'&dlng='.$GLOBALS['fxpglobals']['lang'].'&pks_id_referenz='.$ii_afn.'&pks_id_ref_art=3'.fxReturnSessionParams().'\');"';
							}
							$html .= '		<p class="manual_std"><span'.$cadd.' style="border:1px solid grey;padding:2px 8px;border-radius:8px;box-shadow:3px 3px 3px rgba(255,255,255, 0.5) inset, -3px -3px 3px rgba(0,0,0, 0.5) inset, 2px 2px 2px rgba(0,0,0, 0.5);"'.$tadd.'><b class="darkgrey">'.$flit.'</b></span></p>'.$GLOBALS['nl'];
							$button_set=true;
						}
						else
						{
							$html .= '		<p class="manual_std">';
							if($slng)
							{
								$cadd=' class="dintl';
								$tadd=' title="Field &quot;'.$fi['name'].'&quot; ('.$ii_afn.') - Literal ';
								if(strlen($ii_atp))
									$tadd .= '['.$ii_atp.']';
								else
								{
									$cadd .= 'e';
									$tadd .= 'empty';
								}
								$cadd .= '"';
								$tadd .= ': id_referenz='.$ii_afn.', id_refart=3, id_sprache='.$GLOBALS['fxpglobals']['lang'].'" onclick="window.open(\'language.php?mfield='.$ii_atp.'&slng='.$slng.'&dlng='.$GLOBALS['fxpglobals']['lang'].'&pks_id_referenz='.$ii_afn.'&pks_id_ref_art=3'.fxReturnSessionParams().'\');"';
								$html .= str_replace('{hw}','literal',$hi).'<span'.$cadd.$tadd.'>';
							}
							$html .= '<b class="darkgrey">'.$flit.':</b>';
							if($slng)
								$html .= '</span>';
							$html .= '</p>'.$GLOBALS['nl'];
						}

						// ...Field description
						$htxt=extract_help($fbr[$fld]['text_varlang'], $trans, $msnr);
						$html .= '		';
						if($slng)
							$html .= str_replace('{hw}','description',$hi).'<div class="dintl" style="position:relative;" title="[text_varlang] in table &quot;texte&quot;: id_referenz='.$fld.', id_ref_art=3, id_sprache='.$GLOBALS['fxpglobals']['lang'].'" onclick="window.open(\'language.php?mfield=text_varlang&slng='.$slng.'&dlng='.$GLOBALS['fxpglobals']['lang'].'&pks_id_referenz='.$fld.'&pks_id_ref_art=3'.fxReturnSessionParams().'\');">';
						$html .= '<p class="manual_std">'.$htxt.'</p>';
						if($slng)
							$html .= '</div>';
						$html .= $GLOBALS['nl'];
					}
					else if($slng)
					{
						$html .= '	<img class="dinte" src="'.$GLOBALS['gfxpath'].'fl_op.png" style="position:absolute;left:700px;padding:2px;cursor:pointer;" title="Undo [Hide] ('.$hv.')" onclick="var hh=document.getElementById(\'hide\'); if(hh) {hh.value=\'[U]'.$hv.'\';} var rs=document.getElementById(\'resubmit\'); if(rs) {rs.click();}"><div class="dinte" style="position:relative;" onclick="window.open(\'language.php?mfield='.$ii_atp.'&slng='.$slng.'&dlng='.$GLOBALS['fxpglobals']['lang'].'&pks_id_referenz='.$ii_afn.'&pks_id_ref_art=3'.fxReturnSessionParams().'\');"><b>'.$flit.':</b>&nbsp;&nbsp;<i>Field &quot;'.$fi['name'].'&quot; ('.$ii_afn.') hidden</i></div>'.$GLOBALS['nl'];
					}
				}
				$GLOBALS['headline_set']=false;
			}
			$html .= $GLOBALS['nl'];
		}
	}
	if(!$slng && !$rhtml)
		$html .= createTRHelpHeadFoot('');
	else
		$html .= '		</div>'.$GLOBALS['nl'];

	if(!$rhtml)
		$html=dok_tagclass($html);

	// Global path replacement relative to the doc directory
	$gp='"../../GFX/';
	$html=strtr($html, array('"GFX/'=>$gp, '"./GFX/'=>$gp, '"../GFX/'=>$gp));

	// Save help
	if(!$rhtml)
	{
		$rhtml=true;
		if($slng)
			$hfilename=$GLOBALS['tmppath'].'tpfh_'.$GLOBALS['fxpglobals']['lang'].'.htm';
		else
			$hfilename=$GLOBALS['docpath'].substr('00'.$GLOBALS['fxpglobals']['lang'],-2).'/'.substr('000000'.$trans,-6).'.htm';
		$se=fxSave($hfilename, '<!-- © FeRox Management Consulting GmbH & Co. KG -->'.$GLOBALS['nl'].$html);
		if($se)	// Save error?
			$html=-1;
	}

	$GLOBALS['fxpglobals']['lang']=$blng;
	if($slng)
		unset($GLOBALS['intern']);

	if($rhtml)
		return $html;
}

/**
 * ???FUNCTION???
 *
 * @param various $t - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getTRHelpLiteral($t)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$id=(int)$t['eingabefeld'];	// Field ID
	$mnr=(int)$t['nummer'];		// Mask number
	$typ=(int)$t['maskenart'];	// Mask type: 1=Single mask, 2=Single page list, 3=Total list, 4=Single page report, 5=Total report
	$fld=$t['feldname'];		// Field name
	$art=(int)$t['anzeigeart'];	// Field type: 1=Literal_5, ...
//echo('$mnr='.$mnr.', $id='.$id.', $typ='.$typ.', $fld='.$fld.', $art='.$art.': zeile='.$t['zeile'].', spalte='.$t['spalte'].'<br />');

	$lg='';
	$ai=(int)$t['literal_overwrite'];
	if($ai < 0)
		$lg='*'.meldung(-$ai,true);
	else if($ai == 1360)	// Save
		$lg='*'.get_text($ai,'15');
	else
	{
		if($art < 5)
			$lg='literal_'.($art*5);
		else if($art == 12)
		{
			$ag1=3;
			if($t['anzeigegroesse1'])
			{
				$ag1=$t['anzeigegroesse1'];
				while($ag1 > 5)
					$ag1 -= 5;
			}
			$ag1=min(5,max(1,$ag1));

			if($ag1 == 5)
				$lg='text_kurz';
			else if($ai > 0)
				$lg='*'.get_text($ai,(string)($ag1*5));
			else
				$lg='literal_'.($ag1*5);
		}
		else
		{
			$ag2=(int)$t['anzeigegroesse2'];
			if($ag2 > 0)
			{
				if($ag2 < 5)
					$lg='literal_'.($ag2*5);
				else if($ag2 == 5)
					$lg='literal_text';
				else if(($ag2 != 6) && $fbr[$t['eingabefeld']]['anmerkung_intern'])
					$lg='anmerkung_intern';
				else
					$lg='text_kurz';
			}

			$ag1=0;
			if(($ag2 <= 0) || ($ai > 0))
			{
				$ag1=(int)$t['anzeigegroesse1'];
				if(!$ag1)
				{
					if($t['spaltenanzahl'] > 1)
						$w=(int)$t['spaltenanzahl'];
					else if($t['spaltenanzahl'] < -1)
						$w=abs($t['spaltenanzahl']);
					else
						$w=0;

					$ag1=1+(int)($w/80);
				}
				if($ag1 > 0)
					$ag1=5*min(4,max(1,$ag1));
			}

			if($ag1 > 4)
			{
				if($ai > 0)
					$lg='*'.get_text($ai,(string)$ag1);
				else if(!strlen($lg))
					$lg='literal_'.$ag1;
			}
		}
	}

//echo('&gt; <b>'.$lg.'</b><br />');
	return $lg;
}

/**
 * ???FUNCTION???
 *
 * @param various $filepath - Mandatory parameter:                 ???PARAMETER???
 * @param various $filename - Mandatory parameter:                 ???PARAMETER???
 * @param integer $maxwidth - Optional parameter (default = 628):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dok_screenshot($filepath, $filename, $maxwidth=628)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$img='';
	$src=$filepath.$filename;

	if(file_exists($src))
	{
		// - 0: Breite
		// - 1: Höhe
		// - 2: Grafiktyp (IMAGETYPE_...): 1=GIF, 2=JPG, 3=PNG, 4=SWF, 5=PSD, 6=BMP, 7=TIFF(intel byte order), 8=TIFF(motorola byte order), 9=JPC, 10=JP2, 11=JPX, 12=JB2, 13=SWC, 14=IFF, 15=WBMP, 16=XBM
		$ims=GetImageSize($src);
//fxDebug($ims, $filename);

		$iwidth=0;
		if(fxIsArray($ims))
			$iwidth=min($ims[0], $maxwidth);

		$img='<img src="'.$filename.'"';
		if($iwidth)
		{
			$iheight=round($iwidth * $ims[1] / $ims[0]);
			$img .= ' height="'.$iheight.'" width="'.$iwidth.'"';
		}
		$img .= ' border="0">';
	}

	return $img;
}

/**
 * Funktion zur Ersetzung von Tags mit Tags als Class
 *
 * @param various $textstring - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $delpar     - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dok_tagclass($textstring, $delpar=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Überschriften erweitern mit class... 
	$textstring=str_ireplace('<h1>','<h1 class="manual_h1">',$textstring);
	$textstring=str_ireplace('<h2>','<h2 class="manual_h2">',$textstring);
	$textstring=str_ireplace('<h3>','<h3 class="manual_h3">',$textstring);

	$textstring=str_ireplace('</h1>','</h1><p class="manual_std">',$textstring);
	$textstring=str_ireplace('</h2>','</h2><p class="manual_std">',$textstring);
	$textstring=str_ireplace('</h3>','</h3><p class="manual_std">',$textstring);

	// UL/OL-Listen erweitern mit class... 
	$textstring=str_ireplace('<ul>','<ul class="manual_ul">',$textstring);
	$textstring=str_ireplace('<ol>','<ol class="manual_ol">',$textstring);
	$textstring=str_ireplace('</ul>','</ul><p class="manual_std" style="text-align:justify;">',$textstring);
	$textstring=str_ireplace('</ol>','</ol><p class="manual_std" style="text-align:justify;">',$textstring);

	if($delpar)
		$textstring=dok_delpar($textstring);

	return $textstring;
}

/**
 * Funktion zum Entfernen von <p>...<p>
 *
 * @param various $textstring - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $tagclass   - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dok_delpar($textstring, $tagclass=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$p_pos=strpos($textstring,'</p>');
	if($p_pos)
		$textstring=substr($textstring,0,$p_pos).substr($textstring,$p_pos+4);
	$p_pos=strrpos($textstring,'<p class="manual_std" style="text-align:justify;">');
	if($p_pos)
		$textstring=substr($textstring,0,$p_pos).substr($textstring,$p_pos+49);

	if($tagclass)
		$textstring=dok_tagclass($textstring);

	return $textstring;
}
?>