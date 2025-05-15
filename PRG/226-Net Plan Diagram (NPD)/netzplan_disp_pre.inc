<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : netzplan_disp_pre.inc                                        //
// Version     : 24.1                                                         //
// Begin       : 2021-01-11                                                   //
// Last Change : 2024-03-14                                                   //
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
 * PF 226: Net Plan Diagram (NPD)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

if($disp)
{
	$buffer=max(0, (int)$buffer);

	if(strlen((string)$cpath))
		$cpath=true;
	else
		$cpath=false;

	if(strlen((string)$wd))
		$wd=true;
	else
		$wd=false;

	// Erstellen der Auswahlbedingung
	// ...Mutterprojekt
	$sel['v_top_pid']=$top_pid;
	// ...Puffer
	$sel['v_buffer']=$buffer;
	// ...Krit. Pfad
	$sel['v_cpath']=$cpath;
	// ...Ausrichtung auf Wochentage
	$sel['v_wd']=$wd;
	// ...Planungszeitraum
	$sel['sd']=substr(trim((string)$sdate), 0, 8);
	$sel['ed']=substr(trim((string)$edate), 0, 8);
	// ...Hauptverantwortliche Personen
	$sel['cust']=(int)$cust;
	$sel['pm']=(int)$pm;
	$sel['pl']=(int)$pl;

	// Netzplan in Funktion ermitteln...
	$prj_info_arr=netplan($top_pid, $sel);
fxDebug($prj_info_arr, '$prj_info_arr', 2);

	// ...und anzeigen
	if(fxIsArray($prj_info_arr))
	{
		$sel['prj_info_arr']=$prj_info_arr;
		$GLOBALS['fxptdata']['pdf']=$sel;

		$type_array=kategorie_array('projektart');				// Projektartbezeichnung
		$prio_array=fxf_getReferenceValues('prioritaet');		// Prioritätsbezeichnungen
		$stat_array=fxf_getReferenceValues('projekt_status');	// Projektstatusbezeichnungen
		$weekdays=fxf_getWeekdayNames(2);						// Kurze Wochentagsnamen: 0=So, 1=Mo, ..., 6=Sa


		$txt_open='<i>{'.meldung(1794, false).'}</i>';			// offen
		$txt_nstart='<i>{'.meldung(2833, false).'}</i>';		// nocht nicht begonnen

		$txt_lit=explode('|', meldung(2831, false));			// Vorgangsart|Status|Priorität|Planungszeitraum (Soll)|Arbeitszeitraum (Ist)|Aktionsnachricht
		$leg_lit=explode('|', get_message('NP_Legend'));		// Nr.=Vorgangsnummer|Name=Vorgangsbezeichnung|D=Dauer (in Tagen)|GP=Gesamtpuffer (in Tagen)|FP=freier Puffer (in Tagen)|FB=frühester Beginn (in Tagen nach Projektstart)|FE=frühestes Ende (in Tagen nach Projektstart)|SB=spätester Beginn (in Tagen nach Projektstart)|SE=spätestes Ende (in Tagen nach Projektstart)

		$leg_arr=array(
			array('dk'=>"nr", 'dl'=>"", 'dt'=>""),
			array('dk'=>"name", 'dl'=>"", 'dt'=>""),
			array('dk'=>"len", 'dl'=>"", 'dt'=>""),
			array('dk'=>"gp", 'dl'=>"", 'dt'=>""),
			array('dk'=>"fp", 'dl'=>"", 'dt'=>""),
			array('dk'=>"fb", 'dl'=>"", 'dt'=>""),
			array('dk'=>"fe", 'dl'=>"", 'dt'=>""),
			array('dk'=>"sb", 'dl'=>"", 'dt'=>""),
			array('dk'=>"se", 'dl'=>"", 'dt'=>"")
		);
		foreach($leg_lit as $l => $lit)
		{
			$ep=strpos($lit, '=');
			$leg_arr[$l]['dl']=trim(substr((string)$lit, 0, $ep));
			$leg_arr[$l]['dt']=trim(substr((string)$lit, $ep));
		}
fxDebug($leg_arr, '$leg_arr', 3);

		$top_vn='';

		$ne_width=190;
		$ne_height=90;
		$ne_padding=16;
		$ne_tgap=18;
		$ne_gap=$ne_tgap*4;
		$ne_tpad=floor($ne_height/4)-7;
		$ne_cwidth=floor($ne_width/3);
		$ne_lheight=floor($ne_height/2) - $ne_tpad - 1;
		$ne_ltop2=$ne_lheight + $ne_tpad + 2;

		$nwidth=2*$ne_padding + 2*$ne_tgap + $prj_info_arr['var']['columns']*$ne_width + ($prj_info_arr['var']['columns']-1)*$ne_gap;
		$nheight=2*$ne_padding + 2*$ne_tgap + $prj_info_arr['var']['lines']*$ne_height + ($prj_info_arr['var']['lines']-1)*$ne_gap;

		$div='<div id="netplan" style="position:relative; width:'.$nwidth.'px; height:'.$nheight.'px; border:0; margin:0; padding:0;">'.$nl;

		$nb_width=$ne_width + 2*$ne_tgap;
		$nb_height=$ne_height + 2*$ne_tgap;

		foreach($prj_info_arr as $vn => $data)
		{
			if($vn != 'var')
			{
				if(!strlen((string)$top_vn))
					$top_vn=$vn;

				// Tooltip
				$lcol='eeeeee';
				$tcol='f8f8f8';
				$ptt  = ' tooltip="&nbsp;<b class=black>'.$data['nr'].'</b>&nbsp;<font class=darkgrey>'.strtr($data['name'], $GLOBALS['caption_strtr']).'</font><br><br>';
				$ptt .= '<table width=560 border=0 cellpadding=2 cellspacing=1>';
				$ptt .= '<tr><td bgcolor=#efefff colspan=4 align=center style=\'border-radius:8px;\'><img src='.$GLOBALS['gfxpath'].'np_l'.$GLOBALS['fxpglobals']['lang'].'.png border=0 vspace=4></td></tr>';
				$ptt .= '<tr><td colspan=4></td></tr>';
				foreach($leg_arr as $l => $ld)
				{
					$dt=$data[$ld['dk']];
					if($l == 1)
						$dt=strtr($dt, $GLOBALS['caption_strtr']);

					if($l < 5)
						$ptt .= '<tr><td bgcolor=#'.$lcol.' nowrap valign=top><b>'.$ld['dl'].'</b></td><td bgcolor=#'.$lcol.' nowrap valign=top>'.$ld['dt'].'</td><td bgcolor=#'.$tcol.' colspan=2 nowrap>'.$dt.'</td></tr>';
					else
					{
						$dat=$data[$ld['dk'].'d'];
						$ptt .= '<tr><td bgcolor=#'.$lcol.' nowrap valign=top><b>'.$ld['dl'].'</b></td><td bgcolor=#'.$lcol.' nowrap valign=top>'.$ld['dt'].'</td><td bgcolor=#'.$tcol.' width=50% nowrap>'.$dt.'</td><td bgcolor=#'.$tcol.' align=center nowrap>(=&nbsp;'.$weekdays[getWeekday($dat)].',&nbsp;'.get_entry($dat, "datum").')</td></tr>';
					}
				}
				$ptt .= '<tr><td colspan=4></td></tr>';
				// ...Vorgangsart
				$ptt .= '<tr><td bgcolor=#'.$lcol.' colspan=2 nowrap valign=top><b>'.$txt_lit[0].'</b>:</td><td bgcolor=#'.$tcol.' colspan=2 nowrap>'.$type_array[$data['ptype']].'</td></tr>';
				// ...Status
				$ptt .= '<tr><td bgcolor=#'.$lcol.' colspan=2 nowrap valign=top><b>'.$txt_lit[1].'</b>:</td><td bgcolor=#'.$tcol.' colspan=2 nowrap>'.$stat_array[$data['stat']].'</td></tr>';
				// ...Priorität
				$ptt .= '<tr><td bgcolor=#'.$lcol.' colspan=2 nowrap valign=top><b>'.$txt_lit[2].'</b>:</td><td bgcolor=#'.$tcol.' colspan=2 nowrap>'.$prio_array[$data['prio']].'</td></tr>';
				// ...Planungszeitraum (Soll)
//				$ptt .= '<tr><td bgcolor=#'.$lcol.' colspan=2 nowrap valign=top><b>'.$txt_lit[3].'</b>:</td><td bgcolor=#'.$tcol.' colspan=2 nowrap>'.get_entry($data['ps'], "datum").' - '.get_entry($data['pe'], "datum").'</td></tr>';
				// ...Arbeitszeitraum (Ist)
				$ptt .= '<tr><td bgcolor=#'.$lcol.' colspan=2 nowrap valign=top><b>'.$txt_lit[4].'</b>:</td><td bgcolor=#'.$tcol.' colspan=2 nowrap>';
				if(strlen((string)$data['as']))
				{
					$ptt .= get_entry($data['as'], "datum").' - ';
					if(strlen((string)$data['ae']))
						$ptt .= get_entry($data['ae'], "datum");
					else
						$ptt .= $txt_open;	// offen
				}
				else
					$ptt .= $txt_nstart;	// noch nicht begonnen
				$ptt .= '</td></tr>';
				$ptt .= '</table>"';

				// Borderattribute
				$ne_background=' background:#ffffff;';
				if(strlen((string)$data['disp']['image']))
					$ne_background=' background:url('.$GLOBALS['gfxpath'].$data['disp']['image'].');';

				$ne_borderstyle=$data['disp']['border-width'].'px '.$data['disp']['border-style'].' #'.$data['disp']['border-color'];

				// Bezeichnung
				$ne_ttpad=$ne_tpad;
				$ne_name=$data['name'];
				if(strlen((string)$ne_name) > 20)
				{
					if(strlen((string)$ne_name) > 40)
						$ne_name=substr((string)$ne_name, 0, 40).'...';
					$ne_name=text_wrap($ne_name, true, 20);
				}
				$ne_ttpad=$ne_tpad - 7*substr_count(strtolower((string)$ne_name), '<br>');

				// Datum
				$fbd='&nbsp;<i class="s2 darkgrey">'.get_entry($data['fbd'], "datum").'</i>';
				$fed='<i class="s2 darkgrey">'.get_entry($data['fed'], "datum").'</i>&nbsp;';
				$sbd='&nbsp;<i class="s2 grey">'.get_entry($data['sbd'], "datum").'</i>';
				$sed='<i class="s2 grey">'.get_entry($data['sed'], "datum").'</i>&nbsp;';

				$nb_top=$ne_padding + $data['line']*$nb_height + $data['line']*2*$ne_tgap;
				$nb_left=$ne_padding + $data['column']*$nb_width + $data['column']*2*$ne_tgap;
				$div .= '	<div id="nb_'.str_replace(".", "", $vn).'" style="position:absolute; top:'.$nb_top.'px; left:'.$nb_left.'px; width:'.($nb_width).'px;height:'.$nb_height.'px;margin:0;padding:0;cursor:help;"'.$ptt.'>'.$nl;

				$div .= '		<div style="position:absolute;top:0;left:0;"><b style="border:1px solid #aaccaa;padding:0 2px;">'.$data['fb'].'</b>'.$fbd.'</div>'.$nl;
				$div .= '		<div style="position:absolute;top:0;right:0;">'.$fed.'<b style="border:1px solid #aaccaa;padding:0 2px;">'.$data['fe'].'</b></div>'.$nl;

				$div .= '		<div id="ne_'.str_replace(".", "", $vn).'" style="position:absolute; top:'.$ne_tgap.'px; left:'.$ne_tgap.'px; width:'.$ne_width.'px; height:'.$ne_height.'px; border:'.$ne_borderstyle.';'.$ne_background.' margin:0; padding:0;">'.$nl;
				$div .= '			<div style="position:absolute; top:1px; left:1px; width:'.$ne_cwidth.'px; height:'.$ne_lheight.'px; border-bottom:1px solid #aaaaaa; border-right:1px solid #aaaaaa; text-align:center; padding-top:'.$ne_tpad.'px;"><b class="s2">'.$data['nr'].'</b></div>'.$nl;
				$div .= '			<div style="position:absolute; top:1px; left:'.($ne_cwidth+1).'px; width:'.($ne_cwidth*2 - 2).'px; height:'.$ne_lheight.'px; border-bottom:1px solid #aaaaaa; text-align:center; padding-top:'.$ne_ttpad.'px; overflow:hidden; white-space:nowrap;"><font class="s2">'.$ne_name.'</font></div>'.$nl;
				$div .= '			<div style="position:absolute; top:'.$ne_ltop2.'px; left:1px; width:'.$ne_cwidth.'px; height:'.($ne_lheight-1).'px; border-right:1px solid #aaaaaa; text-align:center; padding-top:'.$ne_tpad.'px;"><font class="s2">'.$data['len'].'</font></div>'.$nl;
				$div .= '			<div style="position:absolute; top:'.$ne_ltop2.'px; left:'.$ne_cwidth.'px; width:'.$ne_cwidth.'px; height:'.($ne_lheight-1).'px; border-right:1px solid #aaaaaa; text-align:center; padding-top:'.$ne_tpad.'px;"><font class="s2">'.$data['gp'].'</font></div>'.$nl;
				$div .= '			<div style="position:absolute; top:'.$ne_ltop2.'px; left:'.($ne_cwidth*2).'px; width:'.$ne_cwidth.'px; height:'.($ne_lheight-1).'px; text-align:center; padding-top:'.$ne_tpad.'px;"><font class="s2">'.$data['fp'].'</font></div>'.$nl;
				$div .= '		</div>'.$nl;

				$div .= '		<div style="position:absolute;bottom:0;left:0;"><font class="s2" style="border:1px solid #ccaaaa;padding:0 2px;">'.$data['sb'].'</font>'.$sbd.'</div>'.$nl;
				$div .= '		<div style="position:absolute;bottom:0;right:0;">'.$sed.'<font class="s2" style="border:1px solid #ccaaaa;padding:0 2px;">'.$data['se'].'</font></div>'.$nl;

				$div .= '	</div>'.$nl;
			}
		}

		// ... normale Verbindungspfeile
		$ne_gap2=floor($ne_gap/2)-1;
		foreach($prj_info_arr as $vn => $data)
		{
			if(($vn != 'var') && !$data['nxt'] && (!$cpath || !$data['cp']))
			{
				// Gibt es einen Nachfolger?
				$nxt=false;
				foreach($prj_info_arr as $nvn => $ndata)
				{
					if($ndata['parent'] == $vn)
					{
						$nxt=true;
						break;
					}
				}
				if(!$nxt)
				{
					$avn=$vn;
					while(strlen((string)$avn) && strlen((string)$prj_info_arr[$avn]['pre']))
					{
						$pvn=$prj_info_arr[$avn]['pre'];

						$al=$prj_info_arr[$avn]['line'];
						$ac=$prj_info_arr[$avn]['column'];
						$pl=$prj_info_arr[$pvn]['line'];
						$pc=$prj_info_arr[$pvn]['column'];

						$x1=$ne_padding + $ne_tgap + ($pc+1)*$ne_width + $pc*$ne_gap + 2;
						$x2=$ne_padding + $ne_tgap + $ac*$ne_width + $ac*$ne_gap;

						if($pl == $al)
						{
							$y2=$ne_padding + $ne_tgap + ($al+1)*$ne_height + $al*$ne_gap - floor($ne_height/2) + 1;
							$div .= '	<div style="position:absolute; top:'.$y2.'px; left:'.$x1.'px; width:'.($x2-$x1).'px; height:1px; border:0; background-color:#000000; overflow:hidden; white-space:nowrap;"></div>'.$nl;
						}
						else
						{
							$y1=$ne_padding + $ne_tgap + ($pl+1)*$ne_height + $pl*$ne_gap - floor($ne_height/2) + 1;
							$y2=$ne_padding + $ne_tgap + ($al+1)*$ne_height + $al*$ne_gap - floor($ne_height/2) + 1;
							$div .= '	<div style="position:absolute; top:'.$y1.'px; left:'.$x1.'px; width:'.$ne_gap2.'px; height:1px; border:0; background-color:#000000; overflow:hidden; white-space:nowrap;"></div>'.$nl;
							$div .= '	<div style="position:absolute; top:'.$y1.'px; left:'.($x2-$ne_gap2).'px; width:1px; height:'.($y2-$y1).'px; border:0; background-color:#000000; overflow:hidden; white-space:nowrap;"></div>'.$nl;
							$div .= '	<div style="position:absolute; top:'.$y2.'px; left:'.($x2-$ne_gap2).'px; width:'.$ne_gap2.'px; height:1px; border:0; background-color:#000000; overflow:hidden; white-space:nowrap;"></div>'.$nl;
						}
						$div .= '	<img src="'.$GLOBALS['gfxpath'].'npan.png" style="position:absolute; top:'.($y2-4).'px; left:'.($x2-10).'px; width:10px; height:9px; border:0;">'.$nl;

						$avn=$pvn;
					}
				}
			}
		}

		// ... Verbindungspfeile des Kritischen Pfades
		if($cpath)
		{
			$avn=$prj_info_arr['var']['cp_vn'];
			while(strlen((string)$avn) && strlen((string)$prj_info_arr[$avn]['pre']))
			{
				$pvn=$prj_info_arr[$avn]['pre'];

				$al=$prj_info_arr[$avn]['line'];
				$ac=$prj_info_arr[$avn]['column'];
				$pl=$prj_info_arr[$pvn]['line'];
				$pc=$prj_info_arr[$pvn]['column'];

				$x1=$ne_padding + $ne_tgap + ($pc+1)*$ne_width + $pc*$ne_gap + 3;
				$x2=$ne_padding + $ne_tgap + $ac*$ne_width + $ac*$ne_gap;

				if($pl == $al)
				{
					$y2=$ne_padding + $ne_tgap + ($al+1)*$ne_height + $al*$ne_gap - floor($ne_height/2) + 1;
					$div .= '	<div style="position:absolute; top:'.$y2.'px; left:'.$x1.'px; width:'.($x2-$x1).'px; height:2px; border:0; background-color:#cc4444; overflow:hidden; white-space:nowrap;"></div>'.$nl;
				}
				else
				{
					$y1=$ne_padding + $ne_tgap + ($pl+1)*$ne_height + $pl*$ne_gap - floor($ne_height/2) + 1;
					$y2=$ne_padding + $ne_tgap + ($al+1)*$ne_height + $al*$ne_gap - floor($ne_height/2) + 1;
					$div .= '	<div style="position:absolute; top:'.$y1.'px; left:'.$x1.'px; width:'.$ne_gap2.'px; height:2px; border:0; background-color:#cc4444; overflow:hidden; white-space:nowrap;"></div>'.$nl;
					$div .= '	<div style="position:absolute; top:'.$y1.'px; left:'.($x2-$ne_gap2).'px; width:2px; height:'.($y2-$y1).'px; border:0; background-color:#cc4444; overflow:hidden; white-space:nowrap;"></div>'.$nl;
					$div .= '	<div style="position:absolute; top:'.$y2.'px; left:'.($x2-$ne_gap2).'px; width:'.$ne_gap2.'px; height:2px; border:0; background-color:#cc4444; overflow:hidden; white-space:nowrap;"></div>'.$nl;
				}
				$div .= '	<img src="'.$GLOBALS['gfxpath'].'npac.png" style="position:absolute; top:'.($y2-4).'px; left:'.($x2-10).'px; width:10px; height:10px; border:0;">'.$nl;

				$avn=$pvn;
			}
		}

		$div .=  '</div>'.$nl;

		// Literal: Mutterprojektbezeichnung
		$nv_text1_lit=$prj_info_arr[$top_vn]['nr'].':&nbsp;&nbsp;'.$prj_info_arr[$top_vn]['name'];
		// DIV-PSP in Maske einbinden
		$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']][0]['nv_text1']='~'.$div;

		// Leerzeile mit Top Border
		fxf_maskColumnBorder(array('nv_text1'), 1, -1, $GLOBALS['_maskcounter'], 1, true);
	}
}
?>