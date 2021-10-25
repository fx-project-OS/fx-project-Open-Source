<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : mta_liste_pre.inc                                            //
// Version     : 21.2                                                         //
// Begin       : 2021-01-13                                                   //
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
 * PF 162: Milestone-Trend-Analysis (MTA)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

//fxDebug($mst, $pids);
if(strlen($pids) && fxIsArray($mst))
{
	// Dateiname für MTA-Grafik
	$filename='';

	// Maskenzähler
	$mc=$GLOBALS['_maskcounter'];
	$z=0;

	// Projektdaten ermitteln
	$pdat=array();
	$sql  = "SELECT projekt_id, vorgangsnummer, name_projekt";
	$sql .= " FROM projekte";
	$sql .= " WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND (projekt_id=".$pid." OR projekt_id IN (".$pids."))";
	$sql .= " ORDER BY vorgangsnummer";
	$tmp=db_values($sql, '*');
fxDebug($tmp, $sql, 3);
	if(fxIsArray($tmp))
	{
		foreach($tmp as $t)
		{
			$apid=(int)$t['projekt_id'];
			$pdat[$apid]=array('vorgangsnummer'=>$t['vorgangsnummer'], 'name_projekt'=>$t['name_projekt']);
		}
	}
fxDebug($pdat, 'Projektdaten', 2);

	// Historien-Zeitdaten ermitteln
	$pfin=array();
	$zdat=array();
	$zvon='99999999';
	$zbis='00000000';

	if($GLOBALS['fxpglobals']['tr'] == 251)	// TTA
	{
		$sql  = "SELECT projekt_id, zeitstempel, soll_beginn, soll_ende, ist_beginn, ist_ende";
		$sql .= " FROM zeitdaten_history";
		$sql .= " WHERE zeitart=".FXP_TE_PROJECT." AND projekt_id IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$sql .= " ORDER BY zeitstempel";
	}
	else									// MTA
	{
		$sql  = "SELECT projekt_id, zeitstempel, NULL AS soll_beginn, soll_ende, NULL AS ist_beginn, NULL AS ist_ende";
		$sql .= " FROM meilensteine_history";
		$sql .= " WHERE projekt_id IN (".$pids.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$sql .= " ORDER BY zeitstempel";
	}
	$tmp=db_values($sql, '*');
fxDebug($tmp, $sql, 2);
	if(fxIsArray($tmp))
	{
		foreach($tmp as $t)
		{
			$apid=(int)$t['projekt_id'];
			if(!isset($pfin[$apid]))
			{
				$adtm=substr($t['zeitstempel'],0,8);
				if(strlen($dtm))
				{
					$zvon=min($zvon, $adtm);
					$zbis=max($zbis, $adtm);
				}

				$dtm=substr($t['ist_ende'],0,8);
				if(strlen($dtm))
				{
					$zvon=min($zvon, $dtm);
					$zbis=max($zbis, $dtm);

					$zdat[$apid][$adtm]=$dtm.'*';
					$pfin[$apid]=true;
				}
				else
				{
					$dtm=substr($t['soll_ende'],0,8);
					if(strlen($dtm))
					{
						$zvon=min($zvon, $dtm);
						$zbis=max($zbis, $dtm);

						$zdat[$apid][$adtm]=$dtm;
					}
				}
			}
		}
	}
//fxDebug($zdat, 'Projekthistorienzeitdaten $zdat: $zvon='.$zvon.', $zbis='.$zbis, 0);

	$fa=fxf_getMaskFields(379);
//fxDebug($fa,'$fa', 0);

	// Zeitrahmen und Meilensteinposition ermitteln
	if(sizeof($zdat))
	{
		foreach($zdat as $zpid => $zd)
		{
			ksort($zd);
			foreach($zd as $adtm => $zdtm)
			{
				$abis=substr($zdtm,0,8);
				$ibis=substr($zdtm, 8, 1);
				if(!$mst[$zpid]['end'])
					$mst[$zpid]['end']=$abis;
				if(($ibis == '*') && !$mst[$zpid]['compl'])
					$mst[$zpid]['compl']=$abis;
			}
			$zdat[$zpid]=$zd;
		}
		$rzvon=$zvon;
		$rzbis=$zbis;
		$rtage=fxp_date_diff($rzvon, $rzbis, 'D');
//echo('<b>$rzvon:</b> '.$rzvon.' - <b>$rzbis:</b> '.$rzbis.' == <b>$rtage:</b> '.$rtage.'<br />');

		if($maskenfeldwerte['zeitspanne_von'])
			$zvon=substr($maskenfeldwerte['zeitspanne_von'],0,8);
		if($maskenfeldwerte['zeitspanne_bis'])
			$zbis=substr($maskenfeldwerte['zeitspanne_bis'],0,8);
		else
			$zbis=time_add($zbis,14,'d');
		if($zbis < $zvon)
		{
			$zz=$zvon;
			$zvon=$zbis;
			$zbis=$zz;
		}
fxDebug($zdat, 'Projekthistorienzeitdaten', 3);
fxDebug($mst, 'Meilensteine ('.$pids.')', 2);

		$tage=fxp_date_diff($zvon, $zbis, 'D');
//echo('<b>$zvon:</b> '.$zvon.' - <b>$zbis:</b> '.$zbis.' == <b>$tage:</b> '.$tage.'<br />');

		if($tage)
		{
			// Heutiges Datum
			$heute=substr($GLOBALS['datetime'],0,8);

			// Meldungstexte
			if($GLOBALS['fxpglobals']['tr'] == 251)
				$axis_ty=meldung(1767, true);	// Vorgangstermine
			else
				$axis_ty=meldung(1763, true);	// Meilensteintermine
			$axis_tx=meldung(1764, true);		// Berichtszeitpunkte
			$prog_ed=meldung(1765, true);		// Geschätzter Endtermin
			$prog_ad=meldung(1766, true);		// Tatsächlicher Endtermin

			// Literale überschreiben
			$Ist_End_Dtm_lit=$axis_tx;
			$Soll_End_Dtm_lit=$axis_ty;

			// Kalendereinteilung ermitteln
			$yadd=1;
			if($tage > 600)
			{
				$md=3;	// Jahre
				$yadd=ceil($tage/6000);
			}
			else if($tage > 140)
				$md=2;	// Monate
			else if($tage > 20)
				$md=1;	// Wochen
			else
				$md=0;	// Tage
//echo('<b>$md:</b> '.$md.'<br />');

			// Koordinatensystem geschätzt=670*670 Pixels (genauer Wert wird später errechnet)
			if($_POST['dateigroesse'])
				$mta_size=$_POST['dateigroesse'];
			else
				$mta_size=670;
			// Rahmen (in Pixel)
			$mleft=80;
			$mtop=24;
			$mright=80;
			$mbottom=24;
			// Titelhöhe (in Pixel)
			if(fxIsArray($pdat) && isset($pdat[$pid]))
				$mhead=64;
			else
				$mhead=0;
			// Fusshöhe für Legende (in Pixel)
			$mfh=32;
			$mfz=16;
			$mfoot=$mfh+sizeof($mst)*$mfz;
			// Achsbeschriftungsbreite (in Pixel)
			$awidth=32;
			// Datumsbreite (in Pixel)
			$dwidth=64;
			$dborder=6;
			// Grafikrand (in Pixel)
			$doffset=12;
			// Pixel pro Tag
			$pix_day=$mta_size/$tage;
			$mta_size=ceil($pix_day*$tage);
			// Gesamtbreite und -höhe (in Pixel)
			$width=$mleft+$awidth+$dwidth+$doffset+$mta_size+$mright;
			$height=$mtop+$mhead+$awidth+$dwidth+$doffset+$mta_size+$mfoot+$mbottom;
			// Ursprungsabstand XY (in Pixel)
			$xoff=$mleft+$awidth+$dwidth+$doffset;
			$yoff=$mtop+$mhead+$awidth+$dwidth+$doffset;
			// Halbe Fontgrösse
			$fh2=7;
//echo('<b>$pix_day:</b> '.$pix_day.' - <b>$mta_size:</b> '.$mta_size.' => <b>$width:</b> '.$width.' x <b>$height:</b> '.$height.'<HR>');

			// Datumsgitter
			$mo_tage=array(1 => 31, 28, 31, 30,  31,  30,  31,  31,  30,  31,  30,  31);
			$lj=fx_is_leap_year(substr($zvon,0,4));
			if($lj)
				$mo_tage[2]=29;

			$adt=$zvon;
//echo('$adt='.$adt.', $zbis='.$zbis.'<HR>');
			while($adt <= $zbis)
			{
				if($adt > $zvon)
					$add=fxp_date_diff($zvon, $adt, 'D');
				else
					$add=0;
				$tag_arr[$adt]=$add;
//echo($ec.': <b>$add:</b> '.$add.' -> <b>$adt:</b> '.$adt.'<br />');

				if($adt == $zbis)
					break;

				$y=(int)substr($adt,0,4);
				$m=(int)substr($adt,4,2);
				$d=(int)substr($adt,6,2);

				if($md == 3)		// Jahre
				{
					$y += $yadd;
					$m=1;
					$d=1;
				}
				else if($md == 2)	// Monate
				{
					$m++;
					$d=1;
					if($m > 12)
					{
						$m=1;
						$y++;
					}
				}
				else
				{

					if($md == 1)	// Wochen
						$d += 7;
					else			// Tage
						$d++;
					if($d > $mo_tage[$m])
					{
						$d -= $mo_tage[$m];
						$m++;
						if($m > 12)
						{
							$m=1;
							$y++;
							$lj=fx_is_leap_year($y);
							if($lj)
								$mo_tage[2]=29;
							else
								$mo_tage[2]=28;
						}
					}
				}
				$adt=substr('0000'.$y, -4).substr('00'.$m, -2).substr('00'.$d, -2);
				if($adt > $zbis)
					$adt=$zbis;
			}
//fxDebug($tag_arr, '$tag_arr');

			// Bild erzeugen
			$im=imagecreatetruecolor($width, $height);
			if($im)
			{
				if(function_exists('imagealphablending'))
					imagealphablending($im, true);

				// Alpha-Blending-Faktor
				$abl=99;

				// Farben
				$bg_col=imagecolorallocate($im, 255, 255, 255);	// Hintegrund (Weiss)
				$wt_col=imagecolorallocate($im, 255, 255, 255);	// Weiss
				$wg_col=imagecolorallocate($im, 240, 240, 240);	// Hellgrau1
				$lg_col=imagecolorallocate($im, 230, 230, 230);	// Hellgrau2
				$dg_col=imagecolorallocate($im, 190, 190, 190);	// Dunkelgrau
				$db_col=imagecolorallocate($im, 0, 107, 159);	// Blau
				$bk_col=imagecolorallocate($im, 64, 64, 64);	// Schwarz

				// Polygonfarbe
				$colarr=array(
					array(121, 174, 199),	// Blau
					array(121, 199, 174),	// Grün
					array(232, 121, 134),	// Rot
					array(232, 121, 232),	// Lila
					array(232, 174, 121),	// Orange
					array(121, 232, 232),	// Türkis
					array(180, 180, 180)	// Grau
				);
				$soc=sizeof($colarr);
				foreach($colarr as $cnr => $col)
				{
					// Hintergrundfarbe für Tabelle
					$tcol[$cnr]=strtoupper(substr('00'.dechex($col[0]), -2).substr('00'.dechex($col[1]), -2).substr('00'.dechex($col[2]), -2));
					// Linien- und Füllfarbe für Polygone
					$pcol[$cnr]=imagecolorallocate($im, $col[0], $col[1], $col[2]);
					// Füllfarbe für Prognosenfläche
					if(function_exists('imagealphablending'))
						$pcola[$cnr]=imagecolorallocatealpha($im, $col[0], $col[1], $col[2], $abl);
					else
						$pcola[$cnr]=imagecolorallocate($im, $col[0], $col[1], $col[2]);
				}

				if(function_exists('imageantialias'))
					imageantialias($im, true);

				imagefilledrectangle($im, 0,0, $width-1,$height-1, $bg_col);

				// Datumslinien
				foreach($tag_arr as $datum => $td)
				{
					// Y-Achse (Hochwert)
					$x=$xoff;
					$y=$yoff+$mta_size-$td*$pix_day;
					imageline($im, $x-$doffset,$y, $x,$y, $bk_col);
					// X-Achse (Rechtswert)
					$x=$xoff+$td*$pix_day;
					$y=$yoff;
					imageline($im, $x,$y-$doffset, $x,$y, $bk_col);

					// Hilfslinien zur Winkelhalbierenden
					$x1=$xoff;
					$y1=$yoff+$mta_size-$td*$pix_day;
					$x2=$xoff+$td*$pix_day;
					$y2=$yoff;
					imageline($im, $x1,$y1, $x2,$y1, $lg_col);
					imageline($im, $x2,$y1, $x2,$y2, $lg_col);

					// Fehlende Zwischenschätzungen nachtragen
					if($datum <= $heute)
					{
						$csdate=array();
						foreach($zdat as $cpid => $czd)
						{
							if(!isset($czd[$datum]))
							{
//echo($cpid.': $datum='.$datum.' -> <b>CALCULATE</b><br />'.$nl);
								foreach($czd as $zb => $zt)
								{
									if(($zb < $datum) && (substr($zt, -1) != '*'))
										$csdate[$cpid]=$zt;
								}
								if(isset($csdate[$cpid]))
								{
									$zdat[$cpid][$datum]=$csdate[$cpid];
									ksort($zdat[$cpid]);
//fxDebug($zdat[$cpid], '$zdat['.$cpid.']');
								}
							}
						}
					}
				}

				// Heute
				$td=fxp_date_diff($zvon, $heute, 'D');
				if(($td > 0) && ($td < $tage))
				{
					// Hilfslinien zur Winkelhalbierenden
					$x1=$xoff;
					$y1=$yoff+$mta_size-$td*$pix_day;
					$x2=$xoff+$td*$pix_day;
					$y2=$yoff;
					imageline($im, $x1,$y1, $x2,$y1, $dg_col);
					imageline($im, $x2,$y1, $x2,$y2, $dg_col);
				}

				// Koordinatensystem
				imageline($im, $xoff,$yoff, $xoff,$yoff+$mta_size, $bk_col);
				imageline($im, $xoff,$yoff, $xoff+$mta_size,$yoff, $bk_col);
				imageline($im, $xoff+$mta_size,$yoff, $xoff,$yoff+$mta_size, $bk_col);

				// Polygonlinien
				$fc=0;
				$pt=0;
				if($_POST['Checkpunkte'])
				{
					if((int)$_POST['Checkpunkte'] < 2)
						$_POST['Checkpunkte']=2;
					$zadd=$tage/$_POST['Checkpunkte'];
//echo('<b>$zadd:</b> '.$zadd.'<br />');
				}

				foreach($mst as $mpid => $m)
				{
					// zuerst geplantes Meilensteindatum
					if($m['epos'])
						$cdate=$m['end'];
					else
						$cdate=$m['start'];

					if($_POST['Tabelle'] && $z)
					{
						$GLOBALS['msk_nbcol_379_'.$z]=$GLOBALS['msk_sbcol'];
						fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
						fxf_maskColumnBorder(array('Nummer'), 0, -1, $mc, $z);
						$z++;
					}

					// Meilensteinname in Tabelle anzeigen
					if($GLOBALS['fxpglobals']['tr'] == 251)	// TTA
						$lwt=no_null($pdat[$mpid]['vorgangsnummer']).'&nbsp;-&nbsp;'.$pdat[$mpid]['name_projekt'];
					else			// MTA
						$lwt=$m['name'].':&nbsp;'.no_null($pdat[$mpid]['vorgangsnummer']).'&nbsp;-&nbsp;'.$pdat[$mpid]['name_projekt'];
					if($_POST['Tabelle'])
					{
						fxf_listSectHeader($mc, $z, '&nbsp;<font class="s4b">'.$lwt.'</font>&nbsp;&nbsp;<font class="grey">('.get_entry($cdate, 'datum').')</font>', 'nummer', 'icon', 5, $tcol[$fc], 1);
						$z++;
					}

					$add=fxp_date_diff($zvon,$rzvon,'D');
					if($rtage < $add)
						$rtage=$add;
//echo('$zvon='.$zvon.', $rzvon='.$rzvon.' -- $add='.$add.', $rtage='.$rtage.'<br />');
					$yadd=$add;

					$ladd=0;			// Letzter Anzeigepunkt
					$cc=0;				// Zähler für Koordinaten und Prognose

					$ed=0;				// Koordinaten-Abstand für Endtermin
					$estimate=true;		// Endtermin ist eine Schätzung?

					$coords=array();	// Koordinaten-Array
					$prog=array();		// Prognose-Array

					$embreak=0;
					while($add <= $rtage)
					{
//echo('while: $add ('.$add.') <= $rtage ('.$rtage.')<br />');
						$rmadd=$add;
						if(!$add)
							$sdate=$zvon;
						else
							$sdate=substr(time_add($zvon, (int)$add, 'D'),0,8);
						$mdate=mta_act_date($mpid, $m['epos'], $sdate);
						if($mdate && (substr($mdate,8,1) == '*'))
						{
							$mdate=substr($mdate,0,8);
							$edate=$mdate;
						}
						else
							$edate='';

						if(!$mdate && $yadd && fxIsArray($zdat[$mpid]))
							$oyadd=$yadd;
						else
							$oyadd='';

						if($mdate || strlen($oyadd))
						{
							if(strlen($oyadd))
								$yadd=$oyadd;
							else
								$yadd=fxp_date_diff($zvon, $mdate, 'D');

							if($edate)
								$add=$yadd;

							$coords[$cc]=array('x'=>$xoff+$add*$pix_day, 'y'=>$yoff+$mta_size-$yadd*$pix_day);
							$prog[$cc]=array('x'=>$add, 'y'=>$yadd);
							$ladd=$add;

							// Tabellenwerte füllen
							if($_POST['Tabelle'])
							{
								mta_table($mc, $z, $cc, $sdate, $cdate, $mdate);
								$z++;
							}

							if($edate)
							{
								$estimate=false;
								$ed=$yadd;
							}

							$cc++;
						}

						if($edate)
						{
							$ladd=$rtage;
							break;
						}

						if($_POST['Checkpunkte'])
							$add += $zadd;
						else if(fxIsArray($zdat[$mpid]))
						{
							reset($zdat[$mpid]);
							$ndtm=key($zdat[$mpid]);
							$add=fxp_date_diff($zvon, $ndtm, 'D');
						}
						else
							break;

						if($add == $rmadd)
						{
							$embreak++;
							if($embreak > 100)
							{
//echo('<b>EMERGENCY BREAK!</b><HR>');
								break;
							}
						}
						else
							$embreak=0;
					}

					if(fxIsArray($coords))
					{
						// Linien
						$sc='';
						foreach($coords as $c)
						{
							if($sc)
								imageline($im, $sc['x'],$sc['y'], $c['x'],$c['y'], $pcol[$fc]);
							$sc=$c;
						}
						// Prognose
						$yadd=$prog[$cc-1]['y'];
//fxDebug($prog, 'Prognose: $ladd='.$ladd.', $yadd='.$yadd.', $cc='.$cc);
						if($ladd < $yadd)
						{
							if($m['compl'])		// Meilenstein bereits fertig
							{
								$estimate=false;
								$ed=fxp_date_diff($zvon, $m['compl'], 'D');
								$ex=$xoff+$ed*$pix_day;
								$ey=$yoff+$mta_size-$ed*$pix_day;
								imageline($im, $coords[$cc-1]['x'],$coords[$cc-1]['y'], $ex,$ey, $pcol[$fc]);
								$coords[$cc]=array('x'=>$ex, 'y'=>$ey);

								// Tabellenwerte füllen
								if($_POST['Tabelle'])
								{
									mta_table($mc, $z, $cc, $m['compl'], $cdate, $m['compl']);
									$z++;
								}
							}
							else if($cc == 1)	// Nur ein Punkt, dann Gerade bis zur Hypothenuse
							{
								$ed=$prog[$cc-1]['y'];
								$ex=$xoff+$ed*$pix_day;
								$ey=$yoff+$mta_size-$ed*$pix_day;
								imageline($im, $coords[$cc-1]['x'],$coords[$cc-1]['y'], $ex,$ey, $pcol[$fc]);
							}
							else if($cc > 1)	// Prognosenfläche, wenn mindestens 2 Punkte vorhanden
							{
								$fak=0;
								$ges=0;
								for($p=1; $p<$cc; $p++)
								{
									$ges += ($prog[$p]['y']-$prog[$p-1]['y'])*$p;
									$fak += $p;
//echo('<b>$ges:</b> '.$ges.', <b>$fak:</b> '.$fak.'<br />');
								}
								$a1=($yadd-$ladd)*$pix_day;
								$ges=$ges/$fak;
//echo('-> <b>$a1:</b> '.$a1.', <b>$ges:</b> '.$ges.', <b>$ladd:</b> '.$ladd.'<br />');
								if($ladd)
									$alpha=atan($ges/$ladd);
								else
									$alpha=atan($ges);
								$malpha=max(min(deg2rad(10), $alpha), deg2rad(-90));
//echo('-> <b>$alpha:</b> '.$alpha.' ('.rad2deg($alpha).'°), <b>$malpha:</b> '.$malpha.' ('.rad2deg($malpha).'°)<br />');
								$s=($a1*sin($malpha)) / (sin(deg2rad(45)-$malpha));
//echo('-> <b>$s:</b> '.$s.'<br />');
								$a2=sqrt(($s*$s)/2);
								if($s < 0)
									$a2=-$a2;
//echo('-> <b>$a2:</b> '.$a2.'<br />');
								$ed=($coords[$cc-1]['x']-$xoff+$a1+$a2/2)/$pix_day;
//echo('-> <b>$ed:</b> '.$ed.'<br />');
								$pcoords=array($coords[$cc-1]['x'],$coords[$cc-1]['y'], $coords[$cc-1]['x']+$a1+$a2,$coords[$cc-1]['y']-$a2, $coords[$cc-1]['x']+$a1,$coords[$cc-1]['y']);
								imagefilledpolygon($im, $pcoords, 3, $pcola[$fc]);
								imagepolygon($im, $pcoords, 3, $pcol[$fc]);
							}
						}
						// Symbol
						foreach($coords as $c)
							gd_symbol($im, $pt, $c['x'],$c['y'], $pcol[$fc], $bk_col);
						// Termin-Endzeit (geschätzt)
						$ex=$xoff+16+$ed*$pix_day;
						$ey=$yoff+$mta_size-4-$ed*$pix_day;
						$ta=time_add($zvon, (int)$ed, 'D');
						$et=get_entry($ta, 'datum');
						if($estimate)
						{
							$et='('.$et.')';
							$tt=$prog_ed;
							$tpc='CECFDA';
						}
						else
						{
							imagerectangle($im, $ex-3,$ey-3, $ex+52,$ey+12, $pcol[$fc]);
							$tt=$prog_ad;
							$tpc='CEEFDA';
						}
						imagestring($im, 1, $ex,$ey, $et, $pcol[$fc]);

						// Tabellenwerte für Prognose füllen
						if($_POST['Tabelle'])
						{
							mta_table($mc, $z, $cc, $ta, $cdate, $ta);

							$GLOBALS['msk_nbcol_379_'.$z]=$GLOBALS['msk_tbcol'];
							$GLOBALS['Nummer_height_'.$z]=42;
							fxf_maskColumnBorder(array('Nummer'), 1, -1, $mc, $z, true);
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ist_end_dtm']='^class_bcr3 bold|~'.$tt.':';
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['soll_end_dtm']='^class_bcr3 bold|~'.str_replace('~','', $GLOBALS['fxptdata']['lvalues'][$mc][$z]['soll_end_dtm']);
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['delta']='^class_bcr3 bold|~'.str_replace('~','', $GLOBALS['fxptdata']['lvalues'][$mc][$z]['delta']);
							$GLOBALS['fxptdata']['lvalues'][$mc][$z]['icon']='^class_bcr3 bold|~'.str_replace('~','', $GLOBALS['fxptdata']['lvalues'][$mc][$z]['icon']);
							$z++;
						}
					}

					$fc++;
					if($fc >= $soc)
						$fc=0;

					$pt++;
					if($pt > 4)
						$pt=0;
				}

				// Ausblenden
				imagefilledrectangle($im, 1,1, $xoff-$doffset, $height-2, $bg_col);						// Links
				imagefilledrectangle($im, 1,1, $width-2, $yoff-$doffset, $bg_col);						// Oben
				imagefilledrectangle($im, 1,$yoff+$mta_size+$doffset, $width-2,$height-2, $bg_col);		// Unten

				// Datum (Text)
				foreach($tag_arr as $datum => $td)
				{
					// Y-Achse (Hochwert)
					$x=$xoff;
					$y=$yoff+$mta_size-$td*$pix_day;
					imagestring($im, 2, $x-$doffset-$dwidth, $y-$fh2, get_entry($datum, 'datum'), $bk_col);
					// X-Achse (Rechtswert)
					$x=$xoff+$td*$pix_day;
					$y=$yoff;
					imagestringup($im, 2, $x-$fh2, $y-$doffset-$dborder, get_entry($datum, 'datum'), $bk_col);
				}

				// Titel
				if(sizeof($pdat) && isset($pdat[$pid]) && $mhead)
				{
					$x1=$mleft;
					$x2=$width-$mright-1;
					$y1=$mtop;
					$y2=$mtop+$mhead-32;
					imagefilledrectangle($im, $x1,$y1, $x2,$y2, $wg_col);
					imagerectangle($im, $x1,$y1, $x2,$y2, $dg_col);
					if(strlen($pdat[$pid]['name_projekt']) > 50)
						$tnp=substr($pdat[$pid]['name_projekt'], 0, 50).'...';
					else
						$tnp=$pdat[$pid]['name_projekt'];
					imagestring($im, 5, $x1+16, $y1+8, no_null($pdat[$pid]['vorgangsnummer']).': '.$tnp, $bk_col);
					imagestring($im, 1, $x2-65, $y1+12, get_entry($heute, 'datum'), $bk_col);
				}

				// Achsbeschriftung X-Achse (Rechtswert): Berichtszeitpunkte
				$x1=$xoff;
				$x2=$xoff+$tage*$pix_day;
				$y=$mtop+$mhead;
				$ac=array($x2,$y, $x2-8,$y-4, $x2-8,$y+4);
				imageline($im, $x1,$y, $x2,$y, $bk_col);
				imagefilledellipse($im, $x1,$y, 6,6, $bk_col);
				imagefilledpolygon($im, $ac, 3, $bk_col);
				imagestring($im, 4, $x1+25, $y-18, $axis_tx, $db_col);
				// Achsbeschriftung Y-Achse (Hochwert): Meilensteintermine
				$at=meldung(1763, true);
				$x=$mleft;
				$y1=$yoff;
				$y2=$yoff+$tage*$pix_day;
				$ac=array($x,$y1, $x-4,$y1+8, $x+4,$y1+8);
				imageline($im, $x,$y1, $x,$y2, $bk_col);
				imagefilledellipse($im, $x,$y2, 6,6, $bk_col);
				imagefilledpolygon($im, $ac, 3, $bk_col);
				imagestringup($im, 4, $x-18, $y2-25, $axis_ty, $db_col);

				// Legende
				$fc=0;
				$pt=0;
				$cz=0;
				foreach($mst as $mpid => $m)
				{
					$x=$xoff;
					$y=$yoff+$mta_size+$mfh+($cz*$mfz);
					// Linie
					imageline($im, $x,$y+6, $x+20,$y+6, $pcol[$fc]);
					// Symbol
					gd_symbol($im, $pt, $x, $y+6, $pcol[$fc], $bk_col);
					// Meilenstein-Name
					if($GLOBALS['fxpglobals']['tr'] == 251)	// TTA
						$lnm=no_null($pdat[$mpid]['vorgangsnummer']).' - '.$pdat[$mpid]['name_projekt'];
					else			// MTA
						$lnm=$m['name'];
					imagestring($im, 3, $x+30, $y, $lnm, $bk_col);

					$fc++;
					if($fc >= $soc)
						$fc=0;

					$pt++;
					if($pt > 4)
						$pt=0;

					$cz++;
				}

				// Bild speichern und in eigener Maske anzeigen
				$filename=$GLOBALS['tmppath'].'~'.fx_date("YmdHis").$GLOBALS['fxpglobals']['person'].'.png';
				imagepng($im, $filename);
				imagedestroy($im);
			}
		}
	}

	// Bild
	if(strlen($filename))
	{
		$png='<img src="'.$filename.'" width="'.$width.'" height="'.$height.'" align="middle">'.$nl;
		// Leerzeile, falls Projektinfos in Tabelle angezeigt wurden
		if($_POST['Tabelle'])
		{
			if($z)
			{
				$GLOBALS['msk_nbcol_379_'.$z]=$GLOBALS['msk_sbcol'];
				fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
				fxf_maskColumnBorder(array('Nummer'), 0, -1, $mc, $z);
				$z++;
			}

			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nummer']='^col_ffffff" nowrap style="border-right:1px solid #'.$GLOBALS['msk_nbcol'].';" align="center" colspan="5"><!--|~'.$png;
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['icon']='~-->'.$png;
		}
		else
		{
			$Nummer_nv=true;
			$Ist_End_Dtm_nv=true;
			$Soll_End_Dtm_nv=true;
			$Delta_nv=true;

			if(fxIsArray($pdat) && isset($pdat[$pid]))
				$icon_lit=no_null($pdat[$pid]['vorgangsnummer']).': '.$pdat[$pid]['name_projekt'];

			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['icon']='~'.$png;
		}
		$z++;

		fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
		$z++;
	}
}


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * ???FUNCTION???
 *
 * @param various $mpid  - Mandatory parameter: ???PARAMETER???
 * @param various $epos  - Mandatory parameter: ???PARAMETER???
 * @param various $datum - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function mta_act_date($mpid, $epos, $datum)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	//echo('function mta_act_date(<b>'.$mpid.'</b>, <b>'.$epos.'</b>, <b>'.$datum.'</b>)...<br />');
	$dat='';
	$rem='';
	if(fxIsArray($GLOBALS['zdat']) && fxIsArray($GLOBALS['zdat'][$mpid]) && strlen($datum))
	{
		foreach($GLOBALS['zdat'][$mpid] as $cdtm => $tdtm)
		{
			if($cdtm > $datum)
				continue;

			$dat=$tdtm;
			$rem=$cdtm;
		}
	}
//echo(' -> $dat=<b>'.$dat.'</b>, $rem=<b>'.$rem.'</b><br />');

	if($rem)
	{
		foreach($GLOBALS['zdat'][$mpid] as $cdtm => $t)
		{
			if($cdtm <= $rem)
				unset($GLOBALS['zdat'][$mpid][$cdtm]);
		}
	}

	return $dat;
}

/**
 * ???FUNCTION???
 *
 * @param various $mc    - Mandatory parameter: ???PARAMETER???
 * @param various $z     - Mandatory parameter: ???PARAMETER???
 * @param various $cc    - Mandatory parameter: ???PARAMETER???
 * @param various $sdate - Mandatory parameter: ???PARAMETER???
 * @param various $cdate - Mandatory parameter: ???PARAMETER???
 * @param various $mdate - Mandatory parameter: ???PARAMETER???
 */
function mta_table($mc, $z, $cc, $sdate, $cdate, $mdate)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Tabellenwerte füllen:
	$dd=fxp_date_diff($cdate, $mdate, 'D');
	if($dd < 0)
	{
		$img='m';
		$fcl='green';
	}
	else if($dd > 0)
	{
		$img='p';
		$fcl='red';
	}
	else
	{
		$img='e';
		$fcl='grey';
	}
	$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nummer']='~'.($cc+1).':';
	$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ist_end_dtm']='~'.get_entry($sdate, 'datum');
	$GLOBALS['fxptdata']['lvalues'][$mc][$z]['soll_end_dtm']='~'.get_entry($mdate, 'datum');
	$GLOBALS['fxptdata']['lvalues'][$mc][$z]['delta']='~<font class="'.$fcl.'">'.get_entry($dd, 'ganzzahl_tz').'</font>&nbsp;<font class="lightgrey">d</d>';
	$GLOBALS['fxptdata']['lvalues'][$mc][$z]['icon']='~<img src="'.$GLOBALS['gfxpath'].'mta'.$img.'.png">';
}

/**
 * ???FUNCTION???
 *
 * @param various &$im - Mandatory parameter: ???PARAMETER???
 * @param various $typ - Mandatory parameter: ???PARAMETER???
 * @param various $x   - Mandatory parameter: ???PARAMETER???
 * @param various $y   - Mandatory parameter: ???PARAMETER???
 * @param various $fc  - Mandatory parameter: ???PARAMETER???
 * @param various $bc  - Mandatory parameter: ???PARAMETER???
 */
function gd_symbol(&$im, $typ, $x, $y, $fc, $bc)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$x1	= $x-4;
	$y1	= $y-4;
	$x2	= $x+4;
	$y2	= $y+4;

	$typ = $typ%5;
	switch($typ)
	{
		case 0:	// Rechteck
			imagefilledrectangle($im, $x1,$y1, $x2,$y2, $fc);
			imagerectangle($im, $x1,$y1, $x2,$y2, $bc);
		break;

		case 1:	// Raute
			$pc = array($x,$y1, $x2,$y, $x,$y2, $x1,$y);
			imagefilledpolygon($im, $pc, 4, $fc);
			imagepolygon($im, $pc, 4, $bc);
		break;

		case 2:	// Dreieck mit Spitze oben
			$pc = array($x,$y1, $x2,$y2, $x1,$y2);
			imagefilledpolygon($im, $pc, 3, $fc);
			imagepolygon($im, $pc, 3, $bc);
		break;

		case 3:	// Dreieck mit Spitze unten
			$pc = array($x1,$y1, $x2,$y1, $x,$y2);
			imagefilledpolygon($im, $pc, 3, $fc);
			imagepolygon($im, $pc, 3, $bc);
		break;

		case 4:	// Kreis
			imagefilledellipse($im, $x,$y, $x2-$x1, $y2-$y1, $fc);
			imageellipse($im, $x,$y, $x2-$x1, $y2-$y1, $bc);
		break;

	}
}

/**
 * ???FUNCTION???
 *
 * @param various $pdat  - Mandatory parameter: ???PARAMETER???
 * @param various $pid   - Mandatory parameter: ???PARAMETER???
 * @param various $datum - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function mta_pln_effort($pdat, $pid, $datum)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!is_array($pdat) || !$pid || !$datum)
		return 0.00;
	if(!is_array($pdat[$pid]))
		return 0.00;

	// Keine Historienwerte, dann aktuellen Wert zurückliefern
	if(!is_array($pdat[$pid]['his']))
	{
//		echo('<B>KEIN Historienwert VORHANDEN</B>, deshalb aktuellen Wert von '.$pdat[$pid]['akt']['aufwand_soll'].' benutzen...<BR>');
		return $pdat[$pid]['akt']['aufwand_soll'];
	}

	$datum = substr($datum, 0, 8);
	foreach($pdat[$pid]['his'] as $hdt => $p)
	{
		if($hdt <= $datum)
		{
//			echo('<B>Historienwert</B> von '.$p['aufwand_soll'].' am '.$hdt.' gefunden...<BR>');
			return $p['aufwand_soll'];
		}
	}

//	echo('<B>KEIN Historienwert GEFUNDEN</B>, deshalb 0.00 benutzen...<BR>');
	return 0.00;
}

/**
 * ???FUNCTION???
 *
 * @param various $idat  - Mandatory parameter: ???PARAMETER???
 * @param various $vn    - Mandatory parameter: ???PARAMETER???
 * @param various $datum - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function mta_act_effort($idat, $vn, $datum)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!is_array($idat) || !$vn || !$datum)
		return 0.00;

	$datum	= substr($datum, 0, 8).'240000';
	$vnlen	= strlen($vn);
	$eff	= 0.00;
	foreach($idat as $i)
	{
		if((substr($i['vorgangsnummer'], 0, $vnlen) == $vn) && ($i['ist_beginn'] <= $datum))
			$eff += $i['zeitaufwand'];
	}
//	echo('<B>Ist-Aufwand</B> suchen für '.$vn.' bis zu '.$datum.' liefert '.$eff.'...<BR>');

	return $eff;
}

/**
 * ???FUNCTION???
 *
 * @param various $pdat  - Mandatory parameter: ???PARAMETER???
 * @param various $pid   - Mandatory parameter: ???PARAMETER???
 * @param various $datum - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function mta_pln_budget($pdat, $pid, $datum)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!is_array($pdat) || !$pid || !$datum)
		return 0.00;
	if(!is_array($pdat[$pid]))
		return 0.00;

	// Keine Historienwerte, dann aktuellen Wert zurückliefern
	if(!is_array($pdat[$pid]['his']))
	{
//		echo('<B>KEIN Historienwert VORHANDEN</B>, deshalb aktuellen Wert von '.$pdat[$pid]['akt']['budget_soll'].' benutzen...<BR>');
		return $pdat[$pid]['akt']['budget_soll'];
	}

	$datum = substr($datum, 0, 8);
	foreach($pdat[$pid]['his'] as $hdt => $p)
	{
		if($hdt <= $datum)
		{
//			echo('<B>Historienwert</B> von '.$p['budget_soll'].' am '.$hdt.' gefunden...<BR>');
			return $p['budget_soll'];
		}
	}

//	echo('<B>KEIN Historienwert GEFUNDEN</B>, deshalb 0.00 benutzen...<BR>');
	return 0.00;
}

/**
 * ???FUNCTION???
 *
 * @param various $idat  - Mandatory parameter: ???PARAMETER???
 * @param various $vn    - Mandatory parameter: ???PARAMETER???
 * @param various $datum - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function mta_act_budget($idat, $vn, $datum)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!is_array($idat) || !$vn || !$datum)
		return 0.00;

	$datum	= substr($datum, 0, 8).'240000';
	$vnlen	= strlen($vn);
	$bud		= 0.00;
	foreach($idat as $i)
	{
		if((substr($i['vorgangsnummer'], 0, $vnlen) == $vn) && ($i['ist_beginn'] <= $datum))
			$bud += $i['kosten_ek'];
	}
//	echo('<B>Ist-Budget</B> suchen für '.$vn.' bis zu '.$datum.' liefert '.$bud.'...<BR>');

	return $bud;
}
?>