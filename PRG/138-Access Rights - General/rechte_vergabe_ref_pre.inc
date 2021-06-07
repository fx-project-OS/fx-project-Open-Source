<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : rechte_vergabe_ref_pre.inc                                   //
// Version     : 21.1                                                         //
// Begin       : 2020-12-21                                                   //
// Last Change : 2020-12-21                                                   //
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
 * PF 138: Access Rights - General
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

$mskfilter=0;
$maske350_sf=true;

// Referenzkategorien
$rca=fxf_getReferenceValues(1311);
//fxDebug($rca,'$rca');

// Auswahlfeld: Referenzkategorie [Rechte_Ref (rcat)]
$Rechte_Ref_nv=false;
if($GLOBALS['fxpglobals']['tr'] != 138)
	$Rechte_Ref_nv=true;
if(!$Rechte_Ref_nv)
{
	$Rechte_Ref_array=array(0=>'<i>'.meldung(684,true).'</i>', -1=>'<d>', 37=>$rca[37], 36=>$rca[36], 38=>$rca[38], 35=>$rca[35]);	// 37=Projekte, 36=Personen, 38=Dokumente + 35=Reports
	$maskenfeldwerte['rechte_ref']=$rcat;
}
echo(fieldHidden('temp_rcat', $rcat));

// Auswahlfeld: (Ordnerlevel je nach Referenzkategorie) [Rechte_ReferenzID (rfld)]
$Rechte_ReferenzID_lit='<b>'.$rca[$rcat].'</b>';

// Auswahlfeld: (Referenz je nach Ordnerlevel der Referenzkategorie) [ID_Referenz (rid)]
$ID_Referenz_lit='<div style="margin-left:24px;margin-right:6px;border-left:1px dotted black;border-bottom:1px dotted black;">&nbsp;</div>';
$ID_Referenz_nc=true;

// Auswahlfelder füllen
$req_fn=fxf_fn_reqFilename('rrefs', 138);
if(strlen($req_fn))
	require($req_fn);

// Filtersymbol anzeigen
$maske350_flt=$mskfilter;
?>