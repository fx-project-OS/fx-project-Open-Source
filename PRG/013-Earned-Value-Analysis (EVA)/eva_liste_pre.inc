<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : eva_liste_pre.inc                                            //
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
 * PF 13: Earned-Value-Analysis (EVA)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

if($show_list)
{
	// Do we have ChartDirector?
	$req_fn=fxf_fn_reqFilename('ext_chartdir');
	if(strlen($req_fn))
		require($req_fn);

	// Auswahlbedingung aufbereiten für Funktionsaufruf
	// ...Projekt
	$sel['top_pid']=$Projekt_ID;
	// ...Hauptverantwortliche Personen
	$sel['cust']=(int)$Kunde;
	$sel['pm']=(int)$Projektmanager;
	$sel['pl']=(int)$Projektleiter;
	// ...Projektstatus
	$sel['pst']='';
	$pst=array('gepla'=>FXP_PS_PLANNED, 'aktiv'=>FXP_PS_ACTIVE, 'inakt'=>FXP_PS_INACTIVE, 'abges'=>FXP_PS_COMPLETED);
	foreach($pst as $pvs => $sti)
	{
		$sel['pst'.$sti]=${'pmenu_'.$pvs};
		if(${'pmenu_'.$pvs})
		{
			if(strlen($sel['pst']))
				$sel['pst'] .= ',';
			$sel['pst'] .= $sti;
		}
	}
//fxDebug($sel, '$sel', 0);

	// EVA in Funktion ermitteln...
	$eva_arr=eva($sel['top_pid'], $sel);
fxDebug($eva_arr, '$eva_arr', 2);

	// Werte gefunden?
	if(fxIsArray($eva_arr) && fxIsArray($eva_arr['pc']))
	{
		// Literale
		$noquot=true;
		$lit_eva=meldung(2865);				// Earned Value Analyse (EVA)
		$lit_ea=meldung(2866);				// Effizienz Analyse (EA)
		$lit_pc=get_text(1967, 'kurz');		// Plankosten (= Planned Cost "PC")
		$lit_ac=get_text(1968, 'kurz');		// Istkosten (= Actual Cost "AC")
		$lit_ev=get_text(1969, 'kurz');		// Leistungswert (= Earned Value "EV")
		$lit_spi=get_text(1972, 'kurz');	// Zeiteffizienz (= Schedule Performance Index "SPI")
		$lit_cpi=get_text(1973, 'kurz');	// Kosteneffizienz (= Cost Performance Index "CPI")
		unset($noquot);

		$fa=fxf_getMaskFields(535);
//fxDebug($fa,'$fa', 0);

		// Falls Tabelle nicht angezeigt werden soll, dann alle Spalten bis auf die letzte ausblenden
		if(!$Tabelle)
		{
			$maske535_nch		= true;	// Keine Spaltenüberschriften

			$Tage_nv			= true;
			$Datum_nv			= true;
			$Aufwand_Soll_nv	= true;
			$Aufwand_Ist_nv		= true;
			$Fertigstellung_nv	= true;
			$pc_nv				= true;
			$ac_nv				= true;
			$ev_nv				= true;
			$sv_nv				= true;
			$cv_nv				= true;
			$spi_nv				= true;
		}

		// Maskenvariablen
		$mc=$GLOBALS['_maskcounter'];
		$z=0;

		// Farben
		$col_black	= '<font class="black">';
		$col_red	= '<font class="red">';
		$col_green	= '<font class="green">';
		$col_grey	= '<font class="grey">';

		// Zahlenformate
		$nkstl=$GLOBALS['fxpglobals']['settings']['nkstellen'];
		$tsdtz='.';
		$deztz=$GLOBALS['fxpglobals']['settings']['deztrennzeichen'];
		if($deztz == '.')
			$tsdtz=',';

		// Währungszeichen
		$currency_list='&nbsp;'.trim($GLOBALS['fxpglobals']['settings']['waehrung']);

		$currency=' '.trim($GLOBALS['fxpglobals']['settings']['waehrung']);
		if($currency == ' ')
			$currency=' Euro';

		// EVA Variablen
		$eva_data=array('date'=>array(), 'pc'=>array(), 'pci'=>array(), 'ac'=>array(), 'aci'=>array(), 'act'=>array(), 'ev'=>array(), 'evi'=>array(), 'evt'=>array(), 'spi'=>array(), 'spii'=>array(), 'spit'=>array(), 'cpi'=>array(), 'cpii'=>array(), 'cpit'=>array());
		$eva_size=sizeof($eva_arr['pc'])-1;

		$fontsize	= 7;

		$cd_width				= 930;
		$cd_height_eva			= 600;
		$cd_height_ea			= 300;

		$cd_border				= 20;

		$cd_titelsize			= 60;
		$cd_legendsize			= 25;

		$cd_valuesize_height	= 40;
		$cd_valuesize_width		= 90;

		$cd_pwidth				= $cd_width - $cd_border - $cd_valuesize_width - $cd_border;
		$cd_pheight_eva			= $cd_height_eva - $cd_border - $cd_titelsize - $cd_legendsize - $cd_valuesize_height - $cd_border;
		$cd_pheight_ea			= $cd_height_ea - $cd_border - $cd_titelsize - $cd_legendsize - $cd_valuesize_height - $cd_border;

		$minpixel_per_date		= 20;
		$pixel_per_day			= $cd_pwidth/$eva_size;
		$dspace					= max(1, round($minpixel_per_date/$pixel_per_day));
//echo('<b>$cd_pwidth</b>='.$cd_pwidth.', <b>$eva_size</b>='.$eva_size.' -- <b>$minpixel_per_date</b>='.$minpixel_per_date.', <b>$pixel_per_day</b>='.$pixel_per_day.' -&gt; <b>$dspace</b>='.$dspace.'<br />');

		$today		= substr(fxNow(), 0, 8);
		$todaycnt	= -1;
		$date_cnt	= 0;
		$date_start	= '';
		$date_end	= '';
		foreach($eva_arr['pc'] as $eva_date => $eva_pc)
		{
			if(!strlen($date_start))
				$date_start=$eva_date;
			$date_end=$eva_date;

			if($eva_date == $today)
				$todaycnt=$date_cnt;

			$eva_data['pc'][$date_cnt]=$eva_pc;

			// Datum <= Heute, dann ermittelte Werte durchgezogen anzeigen
			if($eva_date <= $today)
			{
				$eva_data['ac'][$date_cnt]		= $eva_arr['ac'][$eva_date];
				$eva_data['ev'][$date_cnt]		= $eva_arr['ev'][$eva_date];

				$eva_data['spi'][$date_cnt]		= $eva_arr['spi'][$eva_date];
				$eva_data['cpi'][$date_cnt]		= $eva_arr['cpi'][$eva_date];

				$eva_data['act'][$date_cnt]		= NoValue;
				$eva_data['evt'][$date_cnt]		= NoValue;

				$eva_data['spit'][$date_cnt]	= NoValue;
				$eva_data['cpit'][$date_cnt]	= NoValue;

				if($eva_date == $today)
				{
 					$eva_data['act'][$date_cnt]		= $eva_arr['ac'][$eva_date];
					$eva_data['evt'][$date_cnt]		= $eva_arr['ev'][$eva_date];

					$eva_data['spit'][$date_cnt]	= $eva_arr['spi'][$eva_date];
					$eva_data['cpit'][$date_cnt]	= $eva_arr['cpi'][$eva_date];
				}
			}
			// Datum > Heute, dann Tendenzen gestrichelt anzeigen
			else
			{
				$eva_data['ac'][$date_cnt]		= NoValue;
				$eva_data['ev'][$date_cnt]		= NoValue;

				$eva_data['spi'][$date_cnt]		= NoValue;
				$eva_data['cpi'][$date_cnt]		= NoValue;

					$eva_data['act'][$date_cnt]		= $eva_arr['ac'][$eva_date];
				$eva_data['evt'][$date_cnt]		= $eva_arr['ev'][$eva_date];

				$eva_data['spit'][$date_cnt]	= $eva_arr['spi'][$eva_date];
				$eva_data['cpit'][$date_cnt]	= $eva_arr['cpi'][$eva_date];
			}

			// Punkte in einem gewissen Abstand immer anzeigen
			if(($date_cnt%$dspace == 0) || ($date_cnt == $eva_size))
			{
				$eva_data['pci'][$date_cnt]		= $eva_arr['pc'][$eva_date];
				$eva_data['aci'][$date_cnt]		= $eva_arr['ac'][$eva_date];
				$eva_data['evi'][$date_cnt]		= $eva_arr['ev'][$eva_date];

				$eva_data['spii'][$date_cnt]	= $eva_arr['spi'][$eva_date];
				$eva_data['cpii'][$date_cnt]	= $eva_arr['cpi'][$eva_date];
			}
			else
			{
				$eva_data['pci'][$date_cnt]		= NoValue;
				$eva_data['aci'][$date_cnt]		= NoValue;
				$eva_data['evi'][$date_cnt]		= NoValue;

				$eva_data['spii'][$date_cnt]	= NoValue;
				$eva_data['cpii'][$date_cnt]	= NoValue;
			}

			$date_cnt++;
		}

		$pic_eva='';
		$pic_spi='';
		if(isset($GLOBALS['_ext_chartdir']) && strlen($GLOBALS['_ext_chartdir']))
		{
			$eva_start	= chartTime((int)substr($date_start, 0, 4), (int)substr($date_start, 4, 2), (int)substr($date_start, 6, 2));
			$eva_end	= chartTime((int)substr($date_end, 0, 4), (int)substr($date_end, 4, 2), (int)substr($date_end, 6, 2));
//echo('$dspace='.$dspace.', $date_start='.$date_start.' - $date_end='.$date_end.', $eva_start='.$eva_start.' - $eva_end='.$eva_end.'<br />');

			// ...Dummydaten für Zeitraum
			$space=floor($dspace/2);
			$s_date_start	= time_add($date_start, -$space, 'D');
			$s_date_end		= time_add($date_end, $space, 'D');
			$s_eva_start	= chartTime((int)substr($s_date_start, 0, 4), (int)substr($s_date_start, 4, 2), (int)substr($s_date_start, 6, 2));
			$s_eva_end		= chartTime((int)substr($s_date_end, 0, 4), (int)substr($s_date_end, 4, 2), (int)substr($s_date_end, 6, 2));
//echo('$space='.$space.', $s_date_start='.$s_date_start.' - $s_date_end='.$s_date_end.', $s_eva_start='.$s_eva_start.' - $s_eva_end='.$s_eva_end.'<br />');

			// Linienbreite
			$lw=1;

			// Chartdir-Diagramm für: Earned Value Analyse
			// -------------------------------------------
			// ...Grafikbereich vorbereiten
			$c=new XYChart($cd_width, $cd_height_eva);
			$c->setBackground($c->linearGradientColor(0, 0, 0, 100, 0xe0e0e0, 0xffffff), 0xbbbbbb);
			$c->setRoundedFrame(0xfeffffff);
			$c->setDropShadow(0xfeaaaaaa);
			// ...Diagrammbereich festlegen
			$plotArea=$c->setPlotArea($cd_border+$cd_valuesize_width, $cd_border+$cd_titelsize+$cd_legendsize, $cd_pwidth, $cd_pheight_eva, -1, -1, Transparent, $c->dashLineColor(0xaaaabb, 0x000101), -1);
			// ...Zahlenformat
			$c->setNumberFormat($tsdtz, $deztz, '-');
			// ...Titel
			$titleObj=$c->addTitle($lit_eva."\n", "arialbd.ttf", 16, 0x000000);
			$titleObj->setBackground(0xdedede, 0x000000, glassEffect());
			$c->addTitle("\n\n".no_null($eva_arr['inf']['nr'])." - ".$eva_arr['inf']['name'], "arialbd.ttf", 11, 0x444455);
			// ...Legende
			$legendObj=$c->addLegend($cd_border+$cd_valuesize_width-12, $cd_titelsize, false, '', $fontsize+1);
			$legendObj->setBackground(Transparent);
			// ...X-Achse
			$c->xAxis->setColors(0x444444);
			$c->xAxis->setLabelFormat('{value|'.$GLOBALS['fxpglobals']['settings']['formatdatum'].'}');
			$labelStyleObj=$c->xAxis->setLabelStyle("arial.ttf", $fontsize, 0x000000);
			$labelStyleObj->setFontAngle(60);
			// ...Y-Achse
			$c->yAxis->setColors(0x444444);
			$c->yAxis->setLabelFormat('{value|'.$nkstl.'}'.$currency);
			$c->yAxis->setLabelStyle("arial.ttf", $fontsize, 0x333355);
			// ...Heute (vertikale Linie und "Grauzone")
			if($todaycnt >= 0)
			{
				$xpos=chartTime((int)substr($today, 0, 4), (int)substr($today, 4, 2), (int)substr($today, 6, 2));

				$c->xAxis->addZone($xpos, $s_eva_end, 0xccbbbbbb);

				// Tag (vertikale Linie)
				$xmarkObj=$c->xAxis->addMark($xpos, 0x33448866);
				$xmarkObj->setLineWidth(1.50);
			}
			// ...Linie: Dummydaten
			$lineLayerObjDummy=$c->addSplineLayer();
			$lineLayerObjDummy->setXData2($s_eva_start, $s_eva_end);
			// ...Linie: EV
			$lineLayerObjEV=$c->addSplineLayer();
			$dataSetObjEV=$lineLayerObjEV->addDataSet($eva_data['ev'], 0x1111ee22);
			$lineLayerObjEV->setXData2($eva_start, $eva_end);
			$lineLayerObjEV->setLineWidth($lw);
			// ...Punkte: EV
			$lineLayerObjEVI=$c->addSplineLayer();
			$dataSetObjEVI=$lineLayerObjEVI->addDataSet($eva_data['evi'], 0x1111ee22, $lit_ev);
			$dataSetObjEVI->setDataSymbol(GlassSphere2Shape, 8);
			$lineLayerObjEVI->setXData2($eva_start, $eva_end);
			$lineLayerObjEVI->setLineWidth(0);
			// ...Linie: EVT (Tendenz)
			if($date_end > $today)
			{
				$lineLayerObjEVT=$c->addSplineLayer();
				$dataSetObjEVT=$lineLayerObjEVT->addDataSet($eva_data['evt'], $c->dashLineColor(0x1111ee22, DashLine));
				$lineLayerObjEVT->setXData2($eva_start, $eva_end);
				$lineLayerObjEVT->setLineWidth($lw);
			}
			// ...Linie: AC
			$lineLayerObjAC=$c->addSplineLayer();
			$dataSetObjAC=$lineLayerObjAC->addDataSet($eva_data['ac'], 0x11ee1122);
			$lineLayerObjAC->setXData2($eva_start, $eva_end);
			$lineLayerObjAC->setLineWidth($lw);
			// ...Punkte: AC
			$lineLayerObjACI=$c->addSplineLayer();
			$dataSetObjACI=$lineLayerObjACI->addDataSet($eva_data['aci'], 0x11ee1122, $lit_ac);
			$dataSetObjACI->setDataSymbol(GlassSphere2Shape, 8);
			$lineLayerObjACI->setXData2($eva_start, $eva_end);
			$lineLayerObjACI->setLineWidth(0);
			// ...Linie: ACT (Tendenz)
			if($date_end > $today)
			{
				$lineLayerObjACT=$c->addSplineLayer();
				$dataSetObjACT=$lineLayerObjACT->addDataSet($eva_data['act'], $c->dashLineColor(0x11ee1122, DashLine));
				$lineLayerObjACT->setXData2($eva_start, $eva_end);
				$lineLayerObjACT->setLineWidth($lw);
			}
			// ...Linie: PC
			$lineLayerObjPC=$c->addSplineLayer();
			$dataSetObjPC=$lineLayerObjPC->addDataSet($eva_data['pc'], 0x111122ee);
			$lineLayerObjPC->setXData2($eva_start, $eva_end);
			$lineLayerObjPC->setLineWidth($lw);
			// ...Punkte: PC
			$lineLayerObjPCI=$c->addSplineLayer();
			$dataSetObjPCI=$lineLayerObjPCI->addDataSet($eva_data['pci'], 0x111122ee, $lit_pc);
			$dataSetObjPCI->setDataSymbol(GlassSphere2Shape, 8);
			$lineLayerObjPCI->setXData2($eva_start, $eva_end);
			$lineLayerObjPCI->setLineWidth(0);
			// ...PNG erstellen
			$img=$c->makeChart2(PNG);
			// Diagramm anzeigen
			$fname=saveChartDirectorImage($img, $GLOBALS['tmppath'].'~'.fxNow().$GLOBALS['fxpglobals']['person'].'R'.rand(0,100).'_eva.png');
			if(strlen($fname))
				$pic_eva='<img src="'.$fname.'" border="0" width="'.$cd_width.'" height="'.$cd_height_eva.'"><br />';
			unset($c);

			// Chartdir-Diagramm für: Effizienz Analyse
			// ----------------------------------------
			// ...Grafikbereich vorbereiten
			$c=new XYChart($cd_width, $cd_height_ea);
			$c->setBackground($c->linearGradientColor(0, 0, 0, 100, 0xe0e0e0, 0xffffff), 0xbbbbbb);
			$c->setRoundedFrame(0xfeffffff);
			$c->setDropShadow(0xfeaaaaaa);
			// ...Diagrammbereich festlegen
			$plotArea=$c->setPlotArea($cd_border+$cd_valuesize_width, $cd_border+$cd_titelsize+$cd_legendsize, $cd_pwidth, $cd_pheight_ea, -1, -1, Transparent, $c->dashLineColor(0xaaaabb, 0x000101), -1);
			// ...Zahlenformat
			$c->setNumberFormat($tsdtz, $deztz, '-');
			// ...Titel
			$titleObj=$c->addTitle($lit_ea."\n", "arialbd.ttf", 16, 0x000000);
			$titleObj->setBackground(0xdedede, 0x000000, glassEffect());
			$c->addTitle("\n\n".no_null($eva_arr['inf']['nr'])." - ".$eva_arr['inf']['name'], "arialbd.ttf", 11, 0x444455);
			// ...Legende
			$legendObj=$c->addLegend($cd_border+$cd_valuesize_width-12, $cd_titelsize, false, '', $fontsize+1);
			$legendObj->setBackground(Transparent);
			// ...X-Achse
			$c->xAxis->setColors(0x444444);
			$c->xAxis->setLabelFormat('{value|'.$GLOBALS['fxpglobals']['settings']['formatdatum'].'}');
			$labelStyleObj=$c->xAxis->setLabelStyle("arial.ttf", $fontsize, 0x000000);
			$labelStyleObj->setFontAngle(60);
			// ...Y-Achse
			$c->yAxis->setColors(0x444444);
			$c->yAxis->setLabelFormat('{value|'.$nkstl.'}');
			$c->yAxis->setLabelStyle("arial.ttf", $fontsize, 0x333355);
			// ...Positiver Bereich (>= 1.00) und negativer Bereich (< 1.00)
			$c->yAxis->addZone(-100.00, 1.00, 0xccffaaaa);
			$c->yAxis->addZone(1.00, 100.00, 0xccaaffaa);
			// ...Heute (vertikale Linie)
			if($todaycnt >= 0)
			{
				$xmarkObj=$c->xAxis->addMark($xpos, 0x33448866);
				$xmarkObj->setLineWidth(1.50);
			}
			// ...Linie: Dummydaten
			$lineLayerObjDummy=$c->addSplineLayer();
			$lineLayerObjDummy->setXData2($s_eva_start, $s_eva_end);
			$dataSetObjDummy=$lineLayerObjDummy->addDataSet(array(1, 1), 0xffffffff);
			$lineLayerObjDummy->setLineWidth(0);
			// ...Linie: CPI
			$lineLayerObjCPI=$c->addSplineLayer();
			$dataSetObjCPI=$lineLayerObjCPI->addDataSet($eva_data['cpi'], 0x11ee1122);
			$lineLayerObjCPI->setXData2($eva_start, $eva_end);
			$lineLayerObjCPI->setLineWidth($lw);
			// ...Punkte: CPI
			$lineLayerObjCPII=$c->addSplineLayer();
			$dataSetObjCPII=$lineLayerObjCPII->addDataSet($eva_data['cpii'], 0x11ee1122, $lit_cpi);
			$dataSetObjCPII->setDataSymbol(GlassSphere2Shape, 8);
			$lineLayerObjCPII->setXData2($eva_start, $eva_end);
			$lineLayerObjCPII->setLineWidth(0);
			// ...Linie: CPIT (Tendenz)
			if($date_end > $today)
			{
				$lineLayerObjCPIT=$c->addSplineLayer();
				$dataSetObjCPIT=$lineLayerObjCPIT->addDataSet($eva_data['cpit'], $c->dashLineColor(0x11ee1122, DashLine));
				$lineLayerObjCPIT->setXData2($eva_start, $eva_end);
				$lineLayerObjCPIT->setLineWidth($lw);
			}
			// ...Linie: SPI
			$lineLayerObjSPI=$c->addSplineLayer();
			$dataSetObjSPI=$lineLayerObjSPI->addDataSet($eva_data['spi'], 0x111122ee);
			$lineLayerObjSPI->setXData2($eva_start, $eva_end);
			$lineLayerObjSPI->setLineWidth($lw);
			// ...Punkte: SPI
			$lineLayerObjSPII=$c->addSplineLayer();
			$dataSetObjSPII=$lineLayerObjSPII->addDataSet($eva_data['spii'], 0x111122ee, $lit_spi);
			$dataSetObjSPII->setDataSymbol(GlassSphere2Shape, 8);
			$lineLayerObjSPII->setXData2($eva_start, $eva_end);
			$lineLayerObjSPII->setLineWidth(0);
			// ...Linie: SPIT (Tendenz)
			if($date_end > $today)
			{
				$lineLayerObjSPIT=$c->addSplineLayer();
				$dataSetObjSPIT=$lineLayerObjSPIT->addDataSet($eva_data['spit'], $c->dashLineColor(0x111122ee, DashLine));
				$lineLayerObjSPIT->setXData2($eva_start, $eva_end);
				$lineLayerObjSPIT->setLineWidth($lw);
			}
			// ...PNG erstellen
			$img=$c->makeChart2(PNG);
			// Diagramm anzeigen
			$fname=saveChartDirectorImage($img, $GLOBALS['tmppath'].'~'.fxNow().$GLOBALS['fxpglobals']['person'].'R'.rand(0,100).'_ea.png');
			if(strlen($fname))
				$pic_spi='<img src="'.$fname.'" border="0" width="'.$cd_width.'" height="'.$cd_height_ea.'"><br />';
			unset($c);
		}
		else
			$Tabelle=1;

		// Tabelle
		if($Tabelle)
		{
			// Anzeigeart
			$anz=$Anzeigeart;
			if(!$Anzeigeart)	// Automatisch
			{
				if($date_cnt < 60)			// < 60 Tage
					$anz=917;				// ...Tag(e)
				else if($date_cnt < 420)	// < 420 Tage=60 Wochen
					$anz=1554;			// ...Woche(n)
				else if($date_cnt < 1800)	// < 1800 Tage=60 Monate
					$anz=918;				// ...Monat(e)
				else if($date_cnt < 5400)	// < 5400 Tage=60 Quartale
					$anz=1586;			// ...Quartal(e)
				else						// sonst
					$anz=919;				// ...Jahr(e)
			}
//echo('$Anzeigeart='.$Anzeigeart.' -&gt; $anz='.$anz.'<br />');

			// Wochentag
			$wd=getWeekday($date_start);
			$weekday_names=fxf_getWeekdayNames(2);

			// Alle Einträge durchgehen
			foreach($eva_arr['pc'] as $eva_date => $eva_pc)
			{
				$day	= (int)substr($eva_date, 6, 2);
				$month	= (int)substr($eva_date, 4, 2);
				$year	= (int)substr($eva_date, 0, 4);

				// Prüfen, ob Eintrag angezeigt werden soll
				$display=false;
				// ...Starttag oder Enddtag oder Heute immer anzeigen
				if(($eva_date == $date_start) || ($eva_date == $date_end) || ($eva_date == $today))
					$display=true;
				// ...Tagesweise, dann natürlich jeden Tag anzeigen
				else if($anz == 917)
					$display=true;
				// ...Wochenweise, dann jeden Montag anzeigen
				else if(($anz == 1554) && ($wd == 1))
					$display=true;
				// ...Monatsweise, dann jeden Monatsersten anzeigen
				else if(($anz == 918) && ($day == 1))
					$display=true;
				// ...Quartalsweise, dann jeden Quartalsersten anzeigen
				else if(($anz == 1586) && ($day == 1) && ($month%3 == 1))
					$display=true;
				// ...Jahresweise, dann jeden 1. Januar anzeigen
				else if(($anz == 919) && ($day == 1) && ($month == 1))
					$display=true;

				if($display)
				{
					$eva_pe		= $eva_arr['pe'][$eva_date];
					$eva_ae		= $eva_arr['ae'][$eva_date];
					$eva_pa		= $eva_arr['pa'][$eva_date];
					$eva_ev		= $eva_arr['ev'][$eva_date];
					$eva_ac		= $eva_arr['ac'][$eva_date];
					$eva_sv		= $eva_arr['sv'][$eva_date];
					$eva_cv		= $eva_arr['cv'][$eva_date];
					$eva_spi	= $eva_arr['spi'][$eva_date];
					$eva_cpi	= $eva_arr['cpi'][$eva_date];

					// ...Tag
					$cls='';
					$col=$col_black;
					if($eva_date == $today)
						$cls='^class_rg1|';
					else if($eva_date == $date_start)
						$col=$col_green;
					else if($eva_date == $date_end)
						$col=$col_red;
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['tage']=$cls.'~&nbsp;'.$col.$weekday_names[$wd].'</font>&nbsp;';
					// ...Datum
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['datum']=$cls.'~&nbsp;'.$col.get_entry($eva_date, "datum").'</font>&nbsp;';
					// ...Aufw. Soll
					$val=get_entry($eva_pe/3600.00, "dezimal").'&nbsp;h';
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['aufwand_soll']=$cls.'~&nbsp;'.$val.'&nbsp;';
					// ...Aufw. Ist
					$val=get_entry($eva_ae/3600.00, "dezimal").'&nbsp;h';
					if($eva_date > $today)
						$val=$col_grey.$val.'</font>';
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['aufwand_ist']=$cls.'~&nbsp;'.$val.'&nbsp;';
					// ...S/I
					$val=get_entry($eva_pa*100.00, "dezimal").'&nbsp;%';
					if($eva_date > $today)
						$val=$col_grey.$val.'</font>';
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['fertigstellung']=$cls.'~&nbsp;'.$val.'&nbsp;';
					// ...PC
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['pc']=$cls.'~&nbsp;'.get_entry($eva_pc, "betrag").$currency_list.'&nbsp;';
					// ...AC
					$val=get_entry($eva_ac, "betrag").$currency_list;
					if($eva_date > $today)
						$val=$col_grey.$val.'</font>';
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ac']=$cls.'~&nbsp;'.$val.'&nbsp;';
					// ...EV
					$val=get_entry($eva_ev, "betrag").$currency_list;
					if($eva_date > $today)
						$val=$col_grey.$val.'</font>';
					else
						$val='<b>'.$val.'</b>';
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['ev']=$cls.'~&nbsp;'.$val.'&nbsp;';
					// ...SV
					if($eva_date <= $today)
					{
						$col=$col_green;
						if($eva_sv < 0.00)
							$col=$col_red;
					}
					else
						$col=$col_grey;
					$val=$col.get_entry($eva_sv, "betrag").$currency_list.'</font>';
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['sv']=$cls.'~&nbsp;'.$val.'&nbsp;';
					// ...CV
					if($eva_date <= $today)
					{
						$col=$col_green;
						if($eva_cv < 0.00)
							$col=$col_red;
					}
					else
						$col=$col_grey;
					$val=$col.get_entry($eva_cv, "betrag").$currency_list.'</font>';
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['cv']=$cls.'~&nbsp;'.$val.'&nbsp;';
					// ...SPI
					if($eva_date <= $today)
					{
						$col=$col_green;
						if($eva_spi < 1.00)
							$col=$col_red;
					}
					else
						$col=$col_grey;
					$val=$col.get_entry($eva_spi, "dezimal").'</font>';
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['spi']=$cls.'~&nbsp;'.$val.'&nbsp;';
					// ...CPI
					if($eva_date <= $today)
					{
						$col=$col_green;
						if($eva_cpi < 1.00)
							$col=$col_red;
					}
					else
						$col=$col_grey;
					$val=$col.get_entry($eva_cpi, "dezimal").'</font>';
					$GLOBALS['fxptdata']['lvalues'][$mc][$z]['cpi']=$cls.'~&nbsp;'.$val.'&nbsp;';

					$z++;
				}

				// Wochentag hochzählen: 0=Sonntag, 1=Montag, ..., 6=Samstag
				$wd++;
				if($wd > 6)
					$wd=0;
			}

			fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
			$z++;
		}

		// Diagramm: EVA
		if(strlen($pic_eva))
		{
			$pic='<br />'.$pic_eva;
			if($Tabelle)
			{
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['tage']	= '^class_transparent" nowrap colspan="12" align="center"><!--|~'.$pic;
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['cpi']		= '~-->'.$pic;
			}
			else
			{
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['cpi']	= '^class_transparent|~'.$pic;
			}
			$z++;
		}

		// Diagramm: SPI
		if(strlen($pic_spi))
		{
			$pic='<br />'.$pic_spi;
			if($Tabelle)
			{
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['tage']	= '^class_transparent" nowrap colspan="12" align="center"><!--|~'.$pic;
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['cpi']		= '~-->'.$pic;
			}
			else
			{
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['cpi']	= '^class_transparent|~'.$pic;
			}
			$z++;
		}
	}
}
?>