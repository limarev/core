/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#include <osl/diagnose.h>

#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Sequence.hxx>
#include <svtools/colorcfg.hxx>

#include <global.hxx>
#include <viewopti.hxx>
#include <sc.hrc>
#include <scmod.hxx>
#include <miscuno.hxx>

using namespace utl;
using namespace com::sun::star::uno;



void ScGridOptions::SetDefaults()
{
    *this = ScGridOptions();

    //  grid defaults differ now between the apps
    //  therefore, enter here in its own right (all in 1/100mm)

    if ( ScOptionsUtil::IsMetricSystem() )
    {
        nFldDrawX = 1000;   // 1cm
        nFldDrawY = 1000;
        nFldSnapX = 1000;
        nFldSnapY = 1000;
    }
    else
    {
        nFldDrawX = 1270;   // 0,5"
        nFldDrawY = 1270;
        nFldSnapX = 1270;
        nFldSnapY = 1270;
    }
    nFldDivisionX = 1;
    nFldDivisionY = 1;
}

bool ScGridOptions::operator==( const ScGridOptions& rCpy ) const
{
    return (   nFldDrawX        == rCpy.nFldDrawX
            && nFldDivisionX    == rCpy.nFldDivisionX
            && nFldDrawY        == rCpy.nFldDrawY
            && nFldDivisionY    == rCpy.nFldDivisionY
            && nFldSnapX        == rCpy.nFldSnapX
            && nFldSnapY        == rCpy.nFldSnapY
            && bUseGridsnap     == rCpy.bUseGridsnap
            && bSynchronize     == rCpy.bSynchronize
            && bGridVisible     == rCpy.bGridVisible
            && bEqualGrid       == rCpy.bEqualGrid );
}


ScViewOptions::ScViewOptions()
{
    SetDefaults();
}

ScViewOptions::ScViewOptions( const ScViewOptions& rCpy )
{
    *this = rCpy;
}

ScViewOptions::~ScViewOptions()
{
}

void ScViewOptions::SetDefaults()
{
    aOptArr[ VOPT_FORMULAS     ] = false;
    aOptArr[ VOPT_SYNTAX       ] = false;
    aOptArr[ VOPT_HELPLINES    ] = false;
    aOptArr[ VOPT_GRID_ONTOP   ] = false;
    aOptArr[ VOPT_NOTES        ] = true;
    aOptArr[ VOPT_NULLVALS     ] = true;
    aOptArr[ VOPT_VSCROLL      ] = true;
    aOptArr[ VOPT_HSCROLL      ] = true;
    aOptArr[ VOPT_TABCONTROLS  ] = true;
    aOptArr[ VOPT_OUTLINER     ] = true;
    aOptArr[ VOPT_HEADER       ] = true;
    aOptArr[ VOPT_GRID         ] = true;
    aOptArr[ VOPT_ANCHOR       ] = true;
    aOptArr[ VOPT_PAGEBREAKS   ] = true;
    aOptArr[ VOPT_CLIPMARKS    ] = true;
    aOptArr[ VOPT_SUMMARY      ] = true;
    aOptArr[ VOPT_THEMEDCURSOR ] = false;

    aModeArr[VOBJ_TYPE_OLE ]  = VOBJ_MODE_SHOW;
    aModeArr[VOBJ_TYPE_CHART] = VOBJ_MODE_SHOW;
    aModeArr[VOBJ_TYPE_DRAW ] = VOBJ_MODE_SHOW;

    aGridCol     = SC_STD_GRIDCOLOR;

    aDocCol      = SC_MOD()->GetColorConfig().GetColorValue(svtools::DOCCOLOR).nColor;

    aGridOpt.SetDefaults();
}

Color const & ScViewOptions::GetGridColor( OUString* pStrName ) const
{
    if ( pStrName )
        *pStrName = aGridColName;

    return aGridCol;
}

ScViewOptions& ScViewOptions::operator=(const ScViewOptions& rCpy) = default;

bool ScViewOptions::operator==( const ScViewOptions& rOpt ) const
{
    bool bEqual = true;
    sal_uInt16  i;

    for ( i=0; i<MAX_OPT && bEqual; i++ )  bEqual = (aOptArr [i] == rOpt.aOptArr[i]);
    for ( i=0; i<MAX_TYPE && bEqual; i++ ) bEqual = (aModeArr[i] == rOpt.aModeArr[i]);

    bEqual = bEqual && (aGridCol       == rOpt.aGridCol);
    bEqual = bEqual && (aGridColName   == rOpt.aGridColName);
    bEqual = bEqual && (aGridOpt       == rOpt.aGridOpt);
    bEqual = bEqual && (sColorSchemeName == rOpt.sColorSchemeName);
    bEqual = bEqual && (aDocCol        == rOpt.aDocCol);

    return bEqual;
}

std::unique_ptr<SvxGridItem> ScViewOptions::CreateGridItem() const
{
    std::unique_ptr<SvxGridItem> pItem(new SvxGridItem( SID_ATTR_GRID_OPTIONS ));

    pItem->SetFieldDrawX      ( aGridOpt.GetFieldDrawX() );
    pItem->SetFieldDivisionX  ( aGridOpt.GetFieldDivisionX() );
    pItem->SetFieldDrawY      ( aGridOpt.GetFieldDrawY() );
    pItem->SetFieldDivisionY  ( aGridOpt.GetFieldDivisionY() );
    pItem->SetFieldSnapX      ( aGridOpt.GetFieldSnapX() );
    pItem->SetFieldSnapY      ( aGridOpt.GetFieldSnapY() );
    pItem->SetUseGridSnap   ( aGridOpt.GetUseGridSnap() );
    pItem->SetSynchronize   ( aGridOpt.GetSynchronize() );
    pItem->SetGridVisible   ( aGridOpt.GetGridVisible() );
    pItem->SetEqualGrid     ( aGridOpt.GetEqualGrid() );

    return pItem;
}

//      ScTpViewItem - data for the ViewOptions TabPage

ScTpViewItem::ScTpViewItem( const ScViewOptions& rOpt )
    :   SfxPoolItem ( SID_SCVIEWOPTIONS ),
        theOptions  ( rOpt )
{
}

ScTpViewItem::~ScTpViewItem()
{
}

bool ScTpViewItem::operator==( const SfxPoolItem& rItem ) const
{
    assert(SfxPoolItem::operator==(rItem));

    const ScTpViewItem& rPItem = static_cast<const ScTpViewItem&>(rItem);

    return ( theOptions == rPItem.theOptions );
}

ScTpViewItem* ScTpViewItem::Clone( SfxItemPool * ) const
{
    return new ScTpViewItem( *this );
}

//  Config Item containing view options

constexpr OUStringLiteral CFGPATH_LAYOUT = u"Office.Calc/Layout";

#define SCLAYOUTOPT_GRIDLINES       0
#define SCLAYOUTOPT_GRIDCOLOR       1
#define SCLAYOUTOPT_PAGEBREAK       2
#define SCLAYOUTOPT_GUIDE           3
#define SCLAYOUTOPT_COLROWHDR       4
#define SCLAYOUTOPT_HORISCROLL      5
#define SCLAYOUTOPT_VERTSCROLL      6
#define SCLAYOUTOPT_SHEETTAB        7
#define SCLAYOUTOPT_OUTLINE         8
#define SCLAYOUTOPT_GRID_ONCOLOR    9
#define SCLAYOUTOPT_SUMMARY         10
#define SCLAYOUTOPT_THEMEDCURSOR    11

constexpr OUStringLiteral CFGPATH_DISPLAY = u"Office.Calc/Content/Display";

#define SCDISPLAYOPT_FORMULA        0
#define SCDISPLAYOPT_ZEROVALUE      1
#define SCDISPLAYOPT_NOTETAG        2
#define SCDISPLAYOPT_VALUEHI        3
#define SCDISPLAYOPT_ANCHOR         4
#define SCDISPLAYOPT_TEXTOVER       5
#define SCDISPLAYOPT_OBJECTGRA      6
#define SCDISPLAYOPT_CHART          7
#define SCDISPLAYOPT_DRAWING        8

constexpr OUStringLiteral CFGPATH_GRID = u"Office.Calc/Grid";

#define SCGRIDOPT_RESOLU_X          0
#define SCGRIDOPT_RESOLU_Y          1
#define SCGRIDOPT_SUBDIV_X          2
#define SCGRIDOPT_SUBDIV_Y          3
#define SCGRIDOPT_OPTION_X          4
#define SCGRIDOPT_OPTION_Y          5
#define SCGRIDOPT_SNAPTOGRID        6
#define SCGRIDOPT_SYNCHRON          7
#define SCGRIDOPT_VISIBLE           8
#define SCGRIDOPT_SIZETOGRID        9

Sequence<OUString> ScViewCfg::GetLayoutPropertyNames()
{
    return {"Line/GridLine",            // SCLAYOUTOPT_GRIDLINES
            "Line/GridLineColor",       // SCLAYOUTOPT_GRIDCOLOR
            "Line/PageBreak",           // SCLAYOUTOPT_PAGEBREAK
            "Line/Guide",               // SCLAYOUTOPT_GUIDE
            "Window/ColumnRowHeader",   // SCLAYOUTOPT_COLROWHDR
            "Window/HorizontalScroll",  // SCLAYOUTOPT_HORISCROLL
            "Window/VerticalScroll",    // SCLAYOUTOPT_VERTSCROLL
            "Window/SheetTab",          // SCLAYOUTOPT_SHEETTAB
            "Window/OutlineSymbol",     // SCLAYOUTOPT_OUTLINE
            "Line/GridOnColoredCells",  // SCLAYOUTOPT_GRID_ONCOLOR;
            "Window/SearchSummary",     // SCLAYOUTOPT_SUMMARY
            "Window/ThemedCursor"};     // SCLAYOUTOPT_THEMEDCURSOR
}

Sequence<OUString> ScViewCfg::GetDisplayPropertyNames()
{
    return {"Formula",                  // SCDISPLAYOPT_FORMULA
            "ZeroValue",                // SCDISPLAYOPT_ZEROVALUE
            "NoteTag",                  // SCDISPLAYOPT_NOTETAG
            "ValueHighlighting",        // SCDISPLAYOPT_VALUEHI
            "Anchor",                   // SCDISPLAYOPT_ANCHOR
            "TextOverflow",             // SCDISPLAYOPT_TEXTOVER
            "ObjectGraphic",            // SCDISPLAYOPT_OBJECTGRA
            "Chart",                    // SCDISPLAYOPT_CHART
            "DrawingObject"};           // SCDISPLAYOPT_DRAWING;
}

Sequence<OUString> ScViewCfg::GetGridPropertyNames()
{
    const bool bIsMetric = ScOptionsUtil::IsMetricSystem();

    return {(bIsMetric ? OUString("Resolution/XAxis/Metric")
                       : OUString("Resolution/XAxis/NonMetric")),   // SCGRIDOPT_RESOLU_X
            (bIsMetric ? OUString("Resolution/YAxis/Metric")
                       : OUString("Resolution/YAxis/NonMetric")),   // SCGRIDOPT_RESOLU_Y
             "Subdivision/XAxis",                                   // SCGRIDOPT_SUBDIV_X
             "Subdivision/YAxis",                                   // SCGRIDOPT_SUBDIV_Y
            (bIsMetric ? OUString("Option/XAxis/Metric")
                       : OUString("Option/XAxis/NonMetric")),       // SCGRIDOPT_OPTION_X
            (bIsMetric ? OUString("Option/YAxis/Metric")
                       : OUString("Option/YAxis/NonMetric")),       // SCGRIDOPT_OPTION_Y
             "Option/SnapToGrid",                                   // SCGRIDOPT_SNAPTOGRID
             "Option/Synchronize",                                  // SCGRIDOPT_SYNCHRON
             "Option/VisibleGrid",                                  // SCGRIDOPT_VISIBLE
             "Option/SizeToGrid"};                                  // SCGRIDOPT_SIZETOGRID;
}

ScViewCfg::ScViewCfg() :
    aLayoutItem( CFGPATH_LAYOUT ),
    aDisplayItem( CFGPATH_DISPLAY ),
    aGridItem( CFGPATH_GRID )
{
    sal_Int32 nIntVal = 0;

    Sequence<OUString> aNames = GetLayoutPropertyNames();
    Sequence<Any> aValues = aLayoutItem.GetProperties(aNames);
    aLayoutItem.EnableNotification(aNames);
    const Any* pValues = aValues.getConstArray();
    OSL_ENSURE(aValues.getLength() == aNames.getLength(), "GetProperties failed");
    if(aValues.getLength() == aNames.getLength())
    {
        for(int nProp = 0; nProp < aNames.getLength(); nProp++)
        {
            OSL_ENSURE(pValues[nProp].hasValue(), "property value missing");
            if(pValues[nProp].hasValue())
            {
                switch(nProp)
                {
                    case SCLAYOUTOPT_GRIDCOLOR:
                    {
                        Color aColor;
                        if ( pValues[nProp] >>= aColor )
                            SetGridColor( aColor, OUString() );
                        break;
                    }
                    case SCLAYOUTOPT_GRIDLINES:
                        SetOption( VOPT_GRID, ScUnoHelpFunctions::GetBoolFromAny( pValues[nProp] ) );
                        break;
                    case SCLAYOUTOPT_GRID_ONCOLOR:
                        SetOption( VOPT_GRID_ONTOP, ScUnoHelpFunctions::GetBoolFromAny( pValues[nProp] ) );
                        break;
                    case SCLAYOUTOPT_PAGEBREAK:
                        SetOption( VOPT_PAGEBREAKS, ScUnoHelpFunctions::GetBoolFromAny( pValues[nProp] ) );
                        break;
                    case SCLAYOUTOPT_GUIDE:
                        SetOption( VOPT_HELPLINES, ScUnoHelpFunctions::GetBoolFromAny( pValues[nProp] ) );
                        break;
                    case SCLAYOUTOPT_COLROWHDR:
                        SetOption( VOPT_HEADER, ScUnoHelpFunctions::GetBoolFromAny( pValues[nProp] ) );
                        break;
                    case SCLAYOUTOPT_HORISCROLL:
                        SetOption( VOPT_HSCROLL, ScUnoHelpFunctions::GetBoolFromAny( pValues[nProp] ) );
                        break;
                    case SCLAYOUTOPT_VERTSCROLL:
                        SetOption( VOPT_VSCROLL, ScUnoHelpFunctions::GetBoolFromAny( pValues[nProp] ) );
                        break;
                    case SCLAYOUTOPT_SHEETTAB:
                        SetOption( VOPT_TABCONTROLS, ScUnoHelpFunctions::GetBoolFromAny( pValues[nProp] ) );
                        break;
                    case SCLAYOUTOPT_OUTLINE:
                        SetOption( VOPT_OUTLINER, ScUnoHelpFunctions::GetBoolFromAny( pValues[nProp] ) );
                        break;
                    case SCLAYOUTOPT_SUMMARY:
                        SetOption( VOPT_SUMMARY, ScUnoHelpFunctions::GetBoolFromAny( pValues[nProp] ) );
                        break;
                    case SCLAYOUTOPT_THEMEDCURSOR:
                        SetOption( VOPT_THEMEDCURSOR, ScUnoHelpFunctions::GetBoolFromAny( pValues[nProp] ) );
                        break;
                }
            }
        }
    }
    aLayoutItem.SetCommitLink( LINK( this, ScViewCfg, LayoutCommitHdl ) );

    aNames = GetDisplayPropertyNames();
    aValues = aDisplayItem.GetProperties(aNames);
    aDisplayItem.EnableNotification(aNames);
    pValues = aValues.getConstArray();
    OSL_ENSURE(aValues.getLength() == aNames.getLength(), "GetProperties failed");
    if(aValues.getLength() == aNames.getLength())
    {
        for(int nProp = 0; nProp < aNames.getLength(); nProp++)
        {
            OSL_ENSURE(pValues[nProp].hasValue(), "property value missing");
            if(pValues[nProp].hasValue())
            {
                switch(nProp)
                {
                    case SCDISPLAYOPT_FORMULA:
                        SetOption( VOPT_FORMULAS, ScUnoHelpFunctions::GetBoolFromAny( pValues[nProp] ) );
                        break;
                    case SCDISPLAYOPT_ZEROVALUE:
                        SetOption( VOPT_NULLVALS, ScUnoHelpFunctions::GetBoolFromAny( pValues[nProp] ) );
                        break;
                    case SCDISPLAYOPT_NOTETAG:
                        SetOption( VOPT_NOTES, ScUnoHelpFunctions::GetBoolFromAny( pValues[nProp] ) );
                        break;
                    case SCDISPLAYOPT_VALUEHI:
                        SetOption( VOPT_SYNTAX, ScUnoHelpFunctions::GetBoolFromAny( pValues[nProp] ) );
                        break;
                    case SCDISPLAYOPT_ANCHOR:
                        SetOption( VOPT_ANCHOR, ScUnoHelpFunctions::GetBoolFromAny( pValues[nProp] ) );
                        break;
                    case SCDISPLAYOPT_TEXTOVER:
                        SetOption( VOPT_CLIPMARKS, ScUnoHelpFunctions::GetBoolFromAny( pValues[nProp] ) );
                        break;
                    case SCDISPLAYOPT_OBJECTGRA:
                        if ( pValues[nProp] >>= nIntVal )
                        {
                            //#i80528# adapt to new range eventually
                            if(sal_Int32(VOBJ_MODE_HIDE) < nIntVal) nIntVal = sal_Int32(VOBJ_MODE_SHOW);

                            SetObjMode( VOBJ_TYPE_OLE, static_cast<ScVObjMode>(nIntVal));
                        }
                        break;
                    case SCDISPLAYOPT_CHART:
                        if ( pValues[nProp] >>= nIntVal )
                        {
                            //#i80528# adapt to new range eventually
                            if(sal_Int32(VOBJ_MODE_HIDE) < nIntVal) nIntVal = sal_Int32(VOBJ_MODE_SHOW);

                            SetObjMode( VOBJ_TYPE_CHART, static_cast<ScVObjMode>(nIntVal));
                        }
                        break;
                    case SCDISPLAYOPT_DRAWING:
                        if ( pValues[nProp] >>= nIntVal )
                        {
                            //#i80528# adapt to new range eventually
                            if(sal_Int32(VOBJ_MODE_HIDE) < nIntVal) nIntVal = sal_Int32(VOBJ_MODE_SHOW);

                            SetObjMode( VOBJ_TYPE_DRAW, static_cast<ScVObjMode>(nIntVal));
                        }
                        break;
                }
            }
        }
    }
    aDisplayItem.SetCommitLink( LINK( this, ScViewCfg, DisplayCommitHdl ) );

    ScGridOptions aGrid = GetGridOptions();     //TODO: initialization necessary?
    aNames = GetGridPropertyNames();
    aValues = aGridItem.GetProperties(aNames);
    aGridItem.EnableNotification(aNames);
    pValues = aValues.getConstArray();
    OSL_ENSURE(aValues.getLength() == aNames.getLength(), "GetProperties failed");
    if(aValues.getLength() == aNames.getLength())
    {
        for(int nProp = 0; nProp < aNames.getLength(); nProp++)
        {
            OSL_ENSURE(pValues[nProp].hasValue(), "property value missing");
            if(pValues[nProp].hasValue())
            {
                switch(nProp)
                {
                    case SCGRIDOPT_RESOLU_X:
                        if (pValues[nProp] >>= nIntVal) aGrid.SetFieldDrawX( nIntVal );
                        break;
                    case SCGRIDOPT_RESOLU_Y:
                        if (pValues[nProp] >>= nIntVal) aGrid.SetFieldDrawY( nIntVal );
                        break;
                    case SCGRIDOPT_SUBDIV_X:
                        if (pValues[nProp] >>= nIntVal) aGrid.SetFieldDivisionX( nIntVal );
                        break;
                    case SCGRIDOPT_SUBDIV_Y:
                        if (pValues[nProp] >>= nIntVal) aGrid.SetFieldDivisionY( nIntVal );
                        break;
                    case SCGRIDOPT_OPTION_X:
                        if (pValues[nProp] >>= nIntVal) aGrid.SetFieldSnapX( nIntVal );
                        break;
                    case SCGRIDOPT_OPTION_Y:
                        if (pValues[nProp] >>= nIntVal) aGrid.SetFieldSnapY( nIntVal );
                        break;
                    case SCGRIDOPT_SNAPTOGRID:
                        aGrid.SetUseGridSnap( ScUnoHelpFunctions::GetBoolFromAny( pValues[nProp] ) );
                        break;
                    case SCGRIDOPT_SYNCHRON:
                        aGrid.SetSynchronize( ScUnoHelpFunctions::GetBoolFromAny( pValues[nProp] ) );
                        break;
                    case SCGRIDOPT_VISIBLE:
                        aGrid.SetGridVisible( ScUnoHelpFunctions::GetBoolFromAny( pValues[nProp] ) );
                        break;
                    case SCGRIDOPT_SIZETOGRID:
                        aGrid.SetEqualGrid( ScUnoHelpFunctions::GetBoolFromAny( pValues[nProp] ) );
                        break;
                }
            }
        }
    }
    SetGridOptions( aGrid );
    aGridItem.SetCommitLink( LINK( this, ScViewCfg, GridCommitHdl ) );
}

IMPL_LINK_NOARG(ScViewCfg, LayoutCommitHdl, ScLinkConfigItem&, void)
{
    Sequence<OUString> aNames = GetLayoutPropertyNames();
    Sequence<Any> aValues(aNames.getLength());
    Any* pValues = aValues.getArray();

    for(int nProp = 0; nProp < aNames.getLength(); nProp++)
    {
        switch(nProp)
        {
            case SCLAYOUTOPT_GRIDCOLOR:
                pValues[nProp] <<= GetGridColor();
                break;
            case SCLAYOUTOPT_GRIDLINES:
                pValues[nProp] <<= GetOption( VOPT_GRID );
                break;
            case SCLAYOUTOPT_GRID_ONCOLOR:
                pValues[nProp] <<= GetOption( VOPT_GRID_ONTOP );
                break;
            case SCLAYOUTOPT_PAGEBREAK:
                pValues[nProp] <<= GetOption( VOPT_PAGEBREAKS );
                break;
            case SCLAYOUTOPT_GUIDE:
                pValues[nProp] <<= GetOption( VOPT_HELPLINES );
                break;
            case SCLAYOUTOPT_COLROWHDR:
                pValues[nProp] <<= GetOption( VOPT_HEADER );
                break;
            case SCLAYOUTOPT_HORISCROLL:
                pValues[nProp] <<= GetOption( VOPT_HSCROLL );
                break;
            case SCLAYOUTOPT_VERTSCROLL:
                pValues[nProp] <<= GetOption( VOPT_VSCROLL );
                break;
            case SCLAYOUTOPT_SHEETTAB:
                pValues[nProp] <<= GetOption( VOPT_TABCONTROLS );
                break;
            case SCLAYOUTOPT_OUTLINE:
                pValues[nProp] <<= GetOption( VOPT_OUTLINER );
                break;
            case SCLAYOUTOPT_SUMMARY:
                pValues[nProp] <<= GetOption( VOPT_SUMMARY );
                break;
            case SCLAYOUTOPT_THEMEDCURSOR:
                pValues[nProp] <<= GetOption( VOPT_THEMEDCURSOR );
                break;
        }
    }
    aLayoutItem.PutProperties(aNames, aValues);
}

IMPL_LINK_NOARG(ScViewCfg, DisplayCommitHdl, ScLinkConfigItem&, void)
{
    Sequence<OUString> aNames = GetDisplayPropertyNames();
    Sequence<Any> aValues(aNames.getLength());
    Any* pValues = aValues.getArray();

    for(int nProp = 0; nProp < aNames.getLength(); nProp++)
    {
        switch(nProp)
        {
            case SCDISPLAYOPT_FORMULA:
                pValues[nProp] <<= GetOption( VOPT_FORMULAS );
                break;
            case SCDISPLAYOPT_ZEROVALUE:
                pValues[nProp] <<= GetOption( VOPT_NULLVALS );
                break;
            case SCDISPLAYOPT_NOTETAG:
                pValues[nProp] <<= GetOption( VOPT_NOTES );
                break;
            case SCDISPLAYOPT_VALUEHI:
                pValues[nProp] <<= GetOption( VOPT_SYNTAX );
                break;
            case SCDISPLAYOPT_ANCHOR:
                pValues[nProp] <<= GetOption( VOPT_ANCHOR );
                break;
            case SCDISPLAYOPT_TEXTOVER:
                pValues[nProp] <<= GetOption( VOPT_CLIPMARKS );
                break;
            case SCDISPLAYOPT_OBJECTGRA:
                pValues[nProp] <<= static_cast<sal_Int32>(GetObjMode( VOBJ_TYPE_OLE ));
                break;
            case SCDISPLAYOPT_CHART:
                pValues[nProp] <<= static_cast<sal_Int32>(GetObjMode( VOBJ_TYPE_CHART ));
                break;
            case SCDISPLAYOPT_DRAWING:
                pValues[nProp] <<= static_cast<sal_Int32>(GetObjMode( VOBJ_TYPE_DRAW ));
                break;
        }
    }
    aDisplayItem.PutProperties(aNames, aValues);
}

IMPL_LINK_NOARG(ScViewCfg, GridCommitHdl, ScLinkConfigItem&, void)
{
    const ScGridOptions& rGrid = GetGridOptions();

    Sequence<OUString> aNames = GetGridPropertyNames();
    Sequence<Any> aValues(aNames.getLength());
    Any* pValues = aValues.getArray();

    for(int nProp = 0; nProp < aNames.getLength(); nProp++)
    {
        switch(nProp)
        {
            case SCGRIDOPT_RESOLU_X:
                pValues[nProp] <<= static_cast<sal_Int32>(rGrid.GetFieldDrawX());
                break;
            case SCGRIDOPT_RESOLU_Y:
                pValues[nProp] <<= static_cast<sal_Int32>(rGrid.GetFieldDrawY());
                break;
            case SCGRIDOPT_SUBDIV_X:
                pValues[nProp] <<= static_cast<sal_Int32>(rGrid.GetFieldDivisionX());
                break;
            case SCGRIDOPT_SUBDIV_Y:
                pValues[nProp] <<= static_cast<sal_Int32>(rGrid.GetFieldDivisionY());
                break;
            case SCGRIDOPT_OPTION_X:
                pValues[nProp] <<= static_cast<sal_Int32>(rGrid.GetFieldSnapX());
                break;
            case SCGRIDOPT_OPTION_Y:
                pValues[nProp] <<= static_cast<sal_Int32>(rGrid.GetFieldSnapY());
                break;
            case SCGRIDOPT_SNAPTOGRID:
                pValues[nProp] <<= rGrid.GetUseGridSnap();
                break;
            case SCGRIDOPT_SYNCHRON:
                pValues[nProp] <<= rGrid.GetSynchronize();
                break;
            case SCGRIDOPT_VISIBLE:
                pValues[nProp] <<= rGrid.GetGridVisible();
                break;
            case SCGRIDOPT_SIZETOGRID:
                pValues[nProp] <<= rGrid.GetEqualGrid();
                break;
        }
    }
    aGridItem.PutProperties(aNames, aValues);
}

void ScViewCfg::SetOptions( const ScViewOptions& rNew )
{
    *static_cast<ScViewOptions*>(this) = rNew;
    aLayoutItem.SetModified();
    aDisplayItem.SetModified();
    aGridItem.SetModified();
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
