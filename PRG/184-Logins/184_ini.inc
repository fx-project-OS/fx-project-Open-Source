<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : 184_ini.inc                                                //
// Version     : 21.1                                                         //
// Begin       : 2020-12-22                                                   //
// Last Change : 2020-12-22                                                   //
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
 * PF 184: Logins
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

$p_modus=0;
$a_name='b_protokoll';
if((($_GET['uid'] == 'newtr') && isset($_POST['d_protokoll'])) || $_POST['button_d_protokoll'] || $_POST['Fehler'] || $_POST['Komplett'])
{
	$p_modus=1;
	$a_name='d_protokoll';
}

// Directory-Pfad ermitteln
$filedir=substr($GLOBALS['logpath'],0,-1);
$writedir=$filedir.'/';

// Variablen initialisieren
$directory=array();
$dcnt=0;

// Filenamen im Directory ermitteln
if(is_dir($filedir))
{
	$all=false;
	$years=array();
	$check='_'.$GLOBALS['fxpglobals']['client'].'_';
	if(!$GLOBALS['fxpglobals']['pers'])
		$check='';

	// Directory einlesen im Log-Pfad und nach Protokollen filtern
	$dir_handle=opendir($filedir); // Directory öffnen
	while(false !== ($file = readdir($dir_handle)))
	{
		$filelower=strtolower($file);

		// Directories und Sub-Directories überspringen
		if(($filelower == '.') || ($filelower == '..'))
			continue;
		if(is_dir($writedir.$filelower))
			continue;

		// Alle nicht Log-Files ignorieren
		if(substr($filelower, -4) != '.log')
			continue;

		// Datenbank-Protokolle
		if($p_modus)
		{
			if((substr($filelower,0,3) == '~db') && (substr($filelower,-6,2) != '_i') && (substr_count($filelower,'_') == 2) && (!strlen($check) || (substr($filelower,11,strlen($check)) == $check)))
			{
				$directory[$dcnt] = substr($filelower,1);
				$dcnt++;
			}
		}
		// Benutzer-Protokolle
		else if((strlen($filelower) == 11) && (substr($filelower, 0, 1) == '~'))
		{
			if(!$all)
			{
				$all=true;
				$directory[$dcnt]='['.meldung(1422,false).']';	// Alle;
				$dcnt++;
			}

			$y=(int)substr($filelower,1,4);
			if(!isset($years[$y]))
			{
				$years[$y]=true;
				$directory[$dcnt]=$y.'*';//*999999.log';
				$dcnt++;
			}

			$directory[$dcnt]=substr($filelower,1,4).' '.substr($filelower,5,2);
			$dcnt++;
		}
	}

	// Ergebnis sortieren
	if(fxIsArray($directory))
		rsort($directory,SORT_STRING);

	if(!fxIsArray($directory))
	{
		error_msg(1597, false, '<b>'.$filedir.'</b>'); // DB-Protokollpfad "$var1" enthält keine gültigen Einträge?
		$dbp_err=true;

	}
}
else
{
	error_msg(1596, false, '<b>'.$filedir.'</b>'); // DB-Protokollpfad "$var1" nicht vorhanden?
	$dbp_err=true;
}

if(($_GET['uid'] == 'newtr') || $_POST['button_b_protokoll'] || $_POST['button_d_protokoll'])	// Anzeigemodus wurde gerade aufgerufen oder geändert
{
	$_POST[$a_name]='';
	$sd=sizeof($directory);
	if($sd)
	{
		$_POST[$a_name]=0;
		if((($_GET['uid'] == 'newtr') && isset($_POST['b_protokoll'])) || $_POST['button_b_protokoll'])
		{
			if($sd > 2)
				$_POST[$a_name]=2;
			else if($sd > 1)
				$_POST[$a_name]=1;
		}
	}
}
$oldlocale=setlocale(LC_TIME, 0);
$newlocale=array(1=>'de', 7=>'C');
setlocale(LC_TIME, $newlocale[$GLOBALS['fxpglobals']['lang']]);

// Nicht benötigte Felder ausblenden
// ...Datenbank-Protokolle
if($p_modus)
{
	$b_protokoll_nv			= true;
	$button_d_protokoll_nv	= true;
	$Zeit_ID_nv				= true;
	$Benutzer_ID_nv			= true;
}
// ...Benutzer-Protokolle
else
{
	$d_protokoll_nv			= true;
	$button_b_protokoll_nv	= true;
	$Fehler_nv				= true;
	$Komplett_nv			= true;
}

// Mandanten-Nr.
if(($GLOBALS['fxpglobals']['client'] == 1) || !$GLOBALS['fpxglobals']['pers'])
{
	$Mandanten_ID_ds=1424;	// Alle Mandanten
	$act_client=0;
	if($_POST['Mandanten_ID'])
		$act_client=(int)$_POST['Mandanten_ID'];
}
else
{
	$Mandanten_ID_ds=0;
	$act_client=$GLOBALS['fxpglobals']['client'];
}
$maskenfeldwerte['mandanten_id']=$act_client;

// Sortierfolge
$switch_sort='';
if($_POST['sortierreihe'])
	$switch_sort='1';
$maskenfeldwerte['sortierreihe']=$switch_sort;

// Hidden
$protokoll=(int)$_POST[$a_name];
$maskenfeldwerte['dummy']=$protokoll;

// Auswahlfeld füllen
if(fxIsArray($directory))
{
//fxDebug($directory,'$directory');
	$aa=array();
	$ay=array();
	$ec=0;
	foreach($directory as $dk => $dv)
	{
		if($p_modus)	// Datenbank-Protokolle
		{
			$ae=explode('_',$dv);
			$y=(int)substr($ae[0],2,4);
			if(!isset($ay[$y]))
			{
				$ay[$y]=true;
				if(sizeof($aa))
					$aa[--$ec]='<d>';
				$aa[--$ec]=$y.'<d>';
			}
			$aa[$dk]=$y.'-'.substr($ae[0],6,2).'-'.substr($ae[0],8,2).' ['.substr('00'.$ae[1],-3).']: '.substr($ae[2],0,-4);
		}
		else			// Benutzer-Protokolle
		{
			if($dk)
			{
				if(strlen($dv) < 7)	// Jahr
				{
					$aa[--$ec]='<d>';
					$aa[$dk]=substr($dv,0,4).'<b>';
				}
				else				// Monat im Format z.B. "Januar 2020" anzeigen
					$aa[$dk]='&nbsp;&nbsp;&nbsp;'.fx_strftime("%B %G", zeitsek(substr($dv,0,4).substr($dv,5,2).'01120000'));
			}
			else
				$aa[$dk]='<i>'.$dv.'</i>';
		}
	}
	$GLOBALS[$a_name.'_array']=$aa;
//fxDebug($aa,'$aa');
}
$maskenfeldwerte[$a_name]=$protokoll;

// Button-Texte anpassen (Pfeil vor Standard-Literal-15-Text setzen)
$GLOBALS['button_b_protokoll_btxt']='&rarr; '.get_text(1400, '15');
$GLOBALS['button_d_protokoll_btxt']='&rarr; '.get_text(1401, '15');

// Steuerungsmaske
$maske133_sf=true;
echo(maske(133));


// Protokolldatei auswerten
// ------------------------

// Fileliste erstellen
$filelist=array();
$filecntr=999999;
if(strlen($protokoll) && fxIsArray($directory) && ($_POST['Zeit_ID'] || $_POST['Benutzer_ID'] || $_POST['Fehler'] || $_POST['Komplett']))
{
	$dbfile=$directory[(int)$protokoll];
	if(strlen($dbfile))
	{
		if(substr($dbfile,0,1) == '[')	// Alle
		{
			foreach($directory as $dbfile)
			{
				if((substr($dbfile,0,1) != '[') && (strlen($dbfile) == 7))
					$filelist[$filecntr--]=$writedir.'~'.substr($dbfile,0,4).substr($dbfile,5,2).'.log';
			}
			ksort($filelist);
		}
		else if((strlen($dbfile) == 5) && (substr($dbfile,-1) == '*'))	// Jahr
		{
			$y=(int)substr($dbfile,0,4);
			foreach($directory as $dbfile)
			{
				if((substr($dbfile,0,1) != '[') && (strlen($dbfile) == 7))
				{
					$j=(int)substr($dbfile,0,4);
					if($j === $y)
						$filelist[$filecntr--]=$writedir.'~'.substr($dbfile,0,4).substr($dbfile,5,2).'.log';
				}
			}
			ksort($filelist);
		}
		else if(strlen($dbfile) == 7)
			$filelist[$filecntr]=$writedir.'~'.substr($dbfile,0,4).substr($dbfile,5,2).'.log';
		else
			$filelist[$filecntr]=$writedir.'~'.$dbfile;
	}
}
//fxDebug($filelist, '$filelist');

// Alle Files durchgehen
if(fxIsArray($filelist))
{
	// Datenbank-Protokolle
	if($p_modus)
	{
		$fehlerfilter=false;
		if($Fehler)
			$fehlerfilter=true;

		$db_session=-1;
		$old_session=0;
		$old_subsession=0;
	}
	// Benutzer-Protokolle
	else
	{
		$zeitaufbereitung=false;
		if($Zeit_ID)
			$zeitaufbereitung=true;
	}

	// Array erzeugen
	$dba=array();	// Data array
	$dbc=array();	// Session counter array
	$dbi=array();	// Session timestamp array
	foreach($filelist as $filename)
	{
		if(!file_exists($filename))
			continue;

		$file=file($filename);
//fxDebug($file, $filename);
		// File auswerten
		if(fxIsArray($file))
		{
			// Datenbank-Protokolle
			if($p_modus)
			{
				foreach($file as $entry)
				{
					$entry=trim($entry);
					$fc=substr($entry, 0, 1);
					if($fc == '>')		// Anweisung
					{
						if(substr($entry, 2, 10) == 'Aufgerufen')
							$programm=substr($entry, 26, -1);
					}
					else if($fc == '[')	// DB-Eintrag;
					{
						$new_db_session=(int)substr($entry, 1);
						if($new_db_session != $db_session)
						{
							$old_session=$db_session;
							$db_session=$new_db_session;
						}
						$bracketpos	= strpos($entry, '[', 1);
						$spacepos	= strpos($entry, ' ', $bracketpos);

						$db_subsession	= (int)substr($entry, $bracketpos+1);
						if($old_subsession && ($db_subsession != $old_subsession))
						{
							if(strlen($Dummy) && ((int)$protokoll != (int)$Dummy))
							{
								if($old_session && isset($dba[$old_session]))
									unset($dba[$old_session]);
							}
						}
						$old_subsession	= $db_subsession;
						$uhrzeit		= substr($entry, $spacepos+1, 17);
						$funktion		= substr($entry, $spacepos+22);

						// Rollback nicht anzeigen
						$rbpos=strpos($funktion, 'db_rollback(');
						if($rbpos !== false)
							continue;

						if(strpos($funktion, 'Fehler') !== false)
							$funktion='^'.$funktion;
						$dba[$db_session][$db_subsession]['programm']	= $programm;
						$dba[$db_session][$db_subsession][$uhrzeit]		= $funktion;
//echo('<B>Uhrzeit:</B> '.$uhrzeit.'<BR><B>Funktion:</B> '.$funktion.'<HR SIZE="1">');
					}
				}
			}
			// Benutzer-Protokolle
			else
			{
				$sa=array();
				foreach($file as $entry)
				{
					$entry=trim($entry);
					$datas=explode('|',$entry);
					$datas[0]=explode('_',$datas[0]);
					$datas[6]=explode(',',$datas[6]);
//fxDebug($datas,$entry);

					// Only sysadmin or admin 1 have rights for all clients
					$mid=(int)$datas[0][0];
					if($act_client && ($mid != $act_client))
						continue;

					// Date
					$dt=fx_strftime("%Y%m%d", $datas[1]);

					// ID
					$id=substr('000000'.$datas[0][0],-6).'|'.$datas[5].'|'.$datas[0][1].'|'.$datas[0][2];	// ClientID+PersonName+UserID+PersonID
					if(!isset($dbc[$id]))
						$dbc[$id]=array($dt=>1);
					else if(!isset($dbc[$id][$dt]))
						$dbc[$id][$dt]=1;
					else
						$dbc[$id][$dt]++;

					// Timestamp
					$ts=$datas[1];
					if(!isset($dbi[$ts]))
						$dbi[$ts]=0;
					else
						$dbi[$ts]++;
					$ts .= substr('00'.$dbi[$ts],-2);

					$sa[$ts]=$datas;
					$sa[$ts]['id']=$id;
				}

				if(fxIsArray($sa))
				{
					if(fxIsArray($dbc))
						ksort($dbc);
					// Reverse time
					if($switch_sort)
						krsort($sa);
					else
						ksort($sa);

					// Sort by time
					if($zeitaufbereitung)
					{
						if(!fxIsArray($dba))
							$dba=$sa;
						else
						{
							foreach($sa as $ts => $td)
								$dba[$ts]=$td;
						}
					}
					// Sort by user
					else
					{
						foreach($dbc as $id => $sc)
						{
							if(!fxIsArray($dba[$id]))
								$dba[$id]=array();
							foreach($sa as $ts => $td)
							{
								if($td['id'] == $id)
									$dba[$id][$ts]=$td;
							}
						}
					}
				}
			}
		}
	}
//fxDebug($dbc,'$dbc');
//fxDebug($dba,'$dba');

	// Tabelle erzeugen
	$inh='';
	if(sizeof($dba))
	{
		$z=0;

		// Datenbank-Protokolle
		if($p_modus)
		{
			foreach($dba as $session => $subsession)
			{
				if(strlen($inh))
				{
					$inh .= '<tr><td colspan="2">&nbsp;</td></tr>';
					$z++;
				}

				$inh .= '<tr><td colspan="2" bgcolor="#cccccc" align="left" style="border:1px solid #aaaaaa;"><div style="padding:4px;"><b>Session&nbsp;#'.$session.'</b></div></td></tr>';
				$z++;

				if(fxIsArray($subsession))
				{
					foreach($subsession as $skey => $sdata)
					{
						if(fxIsArray($sdata))
						{
							$inh .= '<tr><td colspan="2" bgcolor="#dddddd" align="left"><div style="padding:4px;">Subsession&nbsp;#'.$skey.'&nbsp;<i class="grey">('.$sdata['programm'].')</i></div></td></tr>';
							$z++;

							$oldclock='';
							foreach($sdata as $uhrzeit => $funktion)
							{
								if(substr($uhrzeit, 0, 1) == 'p')
									continue;
								$fehler=substr($funktion, 0, 1);
								if(($fehler == '^') || !$fehlerfilter)
								{
									if(($fehler == '^') && $fehlerfilter && $oldclock)	// Vorgänger auch anzeigen
									{
										$cls=' class="rv'.($z%2).'"';

										$inh .= '<tr>';
										$inh .= '<td'.$cls.' valign="top" nowrap>&nbsp;<font class="grey">'.$oldclock.'</font>&nbsp;</td>';
										$inh .= '<td'.$cls.' valign="top"><b class="red">'.$sdata[$oldclock].'</b></td>';
										$inh .= '</tr>';
										$z++;
									}

									if($fehler == '^')	// FEHLER
										$funktion=substr($funktion,1);

									$cls=' class="rv'.($z%2).'"';

									$inh .= '<tr>';
									$inh .= '<td'.$cls.' valign="top" nowrap>&nbsp;<font class="grey">'.$uhrzeit.'</font>&nbsp;</td>';
									$inh .= '<td'.$cls.' valign="top"><font class="normal">'.$funktion.'</font></td>';
									$inh .= '</tr>';
									$z++;
								}
								$oldclock=$uhrzeit;
							}
						}
					}
				}
			}
		}

		// Benutzer-Protokolle
		else
		{
			$GLOBALS['mn']=loadMenu(true);

			// ...Sort by time
			if($zeitaufbereitung)
			{
				$odt='';
				foreach($dba as $line)
				{
					$id=$line['id'];
					$ndt=fx_strftime("%Y%m%d", $line[1]);
					if($ndt != $odt)
					{
						$odt=$ndt;
						$sca=array();

						if(strlen($inh))
						{
							$inh .= '<tr><td colspan="10">&nbsp;</td></tr>';
							$z++;
						}

						$title=fx_strftime("%A, ", $line[1]).get_entry($ndt.'120000','datum');
						$inh .= '<tr><td colspan="10" bgcolor="#dddddd" align="left"><div style="padding:4px;">'.$title.'</div></td></tr>';
						$z++;
					}

					if(!isset($sca[$id]))
						$sca[$id]=1;
					else
						$sca[$id]++;

					$sc=$sca[$id];
					if($switch_sort)
						$sc=$dbc[$id][$ndt]+1-$sca[$id];

					$inh .= protLine($z,$sc,$line);
					$z++;
				}
			}
			// Sort by user
			else
			{
				foreach($dba as $id => $sa)
				{
					if(strlen($inh))
					{
						$inh .= '<tr><td colspan="10">&nbsp;</td></tr>';
						$z++;
					}

					$ida=explode('|',$id);
					$inh .= '<tr><td colspan="10" bgcolor="#cccccc" align="left" style="border:1px solid #aaaaaa;border-left:0;border-top:0;"><div style="padding:4px;"><b>'.substr($ida[0],-3).':</b>&nbsp;'.$ida[1].'&nbsp;&nbsp;<i class="grey">('.sizeof($sa).'&nbsp;Sessions)</i></div></td></tr>';
					$z++;

					$sca=0;
					$odt='';

					foreach($sa as $line)
					{
						$sca++;

						$ndt=fx_strftime("%Y%m%d", $line[1]);
						if($ndt != $odt)
						{
							$odt=$ndt;
							$title=fx_strftime("%A, ", $line[1]).get_entry($ndt.'120000','datum');
							$inh .= '<tr><td colspan="10" bgcolor="#dddddd" align="left"><div style="padding:4px;">'.$title.'</div></td></tr>';
							$z++;
						}

						$sc=$sca;
						if($switch_sort)
							$sc=$dbc[$id][$ndt]+1-$sca;

						$inh .= protLine($z,$sc,$line);
						$z++;
					}
				}
			}
		}
	}

	if($inh)
	{
		$msktitle=trim(strtr($aa[(int)$protokoll], array('&nbsp;'=>'', '<b>'=>'')));
		$inh='<table width="952" border="0" cellspacing="0">'.$inh.'</table>';
//function std_mask($title, $inhalt, $width='100%', $height='100%', $align='left', $button='')
		echo(std_mask($msktitle, $inh));
	}
}

// Anzeigeformat zurücksetzen
setlocale(LC_TIME, $oldlocale);


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * ???FUNCTION???
 *
 * @param various $z       - Mandatory parameter:               ???PARAMETER???
 * @param various $sn      - Mandatory parameter:               ???PARAMETER???
 * @param various $line    - Mandatory parameter:               ???PARAMETER???
 * @param integer $maxDays - Optional parameter (default = 4):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function protLine($z,$sn,$line,$maxDays=4)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	list($usec, $sec)=explode(' ', microtime());
	$tdc=$sec.substr($usec,1);
	$tdd=floor($tdc-$line[2]);
	// ...older than maxDays => maxDays * 24 hours/day * 60 min/hour * 60 sec/min=maxDays * 86400 sec/days=604800 sec for e.g. 7 days
	$tdm=floor($maxDays * 86400);

	// Sysadin user?
	$suser=0;
	if(substr($line[4],0,1) == '^')
	{
		$suser=1;
		if($line[0][2] > 0)	// Person ID
			$line[4]='<b class=blue>sys</b> &rarr; <i>'.substr($line[4],1).'</i>';
		else
			$line[4]='<b class=blue>sys</b>';
	}

	$cls=' class="rv'.($z%2).'"';
	$inh='<tr>';
	// Timespan
	$inh .= '<td'.$cls.' align="center" nowrap>&nbsp;<font class="grey">'.fx_strftime("%H:%M:%S", $line[1]).'-'.fx_strftime("%H:%M:%S", $line[2]).'</font>&nbsp;</td>';
	// Client no
	$inh .= '<td'.$cls.' align="right" nowrap>&nbsp;'.$line[0][0].'&nbsp;</td>';
	// Client name
	$inh .= '<td'.$cls.' nowrap>&nbsp;'.$line[3].'&nbsp;</td>';
	// User name
	$inh .= '<td'.$cls.' nowrap>&nbsp;['.$line[4].']&nbsp;</td>';
	// Person name
	$inh .= '<td'.$cls.' nowrap>&nbsp;<b>'.$line[5].'</b>&nbsp;</td>';
	// IP
	$ipr=$line[0][3];
	$ipm=$ipr;
	$ipd=strpos($ipm,'.');
	if($ipd)
		$ipm=substr($ipm,0,$ipd).'.#.#.#';
	else
	{
		$ipd=strpos($ipm,':');
		if($ipd)
			$ipm=substr($ipm,0,$ipd).':#:#:#:#:#';
	}
	if($tdd > $tdm)	// older than maxDays
		$inh .= '<td'.$cls.' align="left" nowrap>&nbsp;<i class="grey">'.$ipm.'</i>&nbsp;</td>';
	else if($GLOBALS['_elinks'])
		$inh .= '<td'.$cls.' align="left" nowrap>&nbsp;<a class="blue" href="http://'.$ipr.'" target="_blank">'.$ipr.'</a>&nbsp;</td>';
	else
		$inh .= '<td'.$cls.' align="left" nowrap>&nbsp;<span class="blue">'.$ipr.'</span>&nbsp;</td>';
	// Session
	$slms=round($line[2]-$line[1],2);
	$col='green';
	if($slms < 300)
		$col='red';
	$inh .= '<td'.$cls.' width="100%" align="right" nowrap>&nbsp;<font class="'.$col.'">Session&nbsp;#'.$sn.'</font>&nbsp;&nbsp;</td>';
	// Session length (s)
	$inh .= '<td'.$cls.' align="right" nowrap>&nbsp;'.get_entry($slms,'dezimal').'&nbsp;s&nbsp;</td>';
	// Session length (h)
	$slt=ceil($slms);
	$slh=max(0,floor($slt/3600));
	$slm=max(0,floor(($slt-($slh*3660))/60));
	$sls=max(0,floor($slt-($slh*3660)-($slm*60)));
	$inh .= '<td'.$cls.' align="right" nowrap>&nbsp;('.$slh.':'.substr('00'.$slm,-2).'´'.substr('00'.$sls,-2).')&nbsp;</td>';
	// Transaction calls
	$tca=array();
	$tt='';
	$tc=0;
	foreach($line[6] as $tl)
	{
		if($tc > 30)
			break;

		$tla=explode('_',$tl);
		if(!isset($tca[$tla[0]]))
			$tca[$tla[0]]=1;
		else
			$tca[$tla[0]]++;

		$tx='?';
		if(fxIsArray($GLOBALS['mn']))
		{
			foreach($GLOBALS['mn'] as $m)
			{
				if($m['tr'] == $tla[0])
				{
					$tx=$m['tx'];
					break;
				}
			}
		}

		if(strlen($tt))
			$tt .= '<br>';
		$tt .= fx_strftime("%H:%M:%S", $tla[1]).'-'.fx_strftime("%H:%M:%S", $tla[2]).'&nbsp;&gt;&nbsp;<b>'.$tla[0].':</b>&nbsp;'.str_replace('"','&quot;',$tx);

		$tc++;
	}
	$inh .= '<td'.$cls.' id="tc'.$z.'" align="right" tooltip="'.$tt.'" style="cursor:help;" nowrap>&nbsp;'.sizeof($tca).'/'.sizeof($line[6]).'&nbsp;TC\'s&nbsp;</td>';
	$inh .= '</tr>';

	return $inh;
}
?>