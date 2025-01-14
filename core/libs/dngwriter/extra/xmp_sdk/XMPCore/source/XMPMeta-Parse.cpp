// =================================================================================================
// Copyright 2003 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. If you have received this file from a source other
// than Adobe, then your use, modification, or distribution of it requires the prior written permission
// of Adobe.
//
// Adobe patent application tracking #P435, entitled 'Unique markers to simplify embedding data of
// one format in a file with a different format', inventors: Sean Parent, Greg Gilley.
// =================================================================================================

#if AdobePrivate
// =================================================================================================
// Change history
// ==============
//
// Writers:
//  AWL Alan Lillich
//  ADC Amandeep Chawla
//
// mm-dd-yy who Description of changes, most recent on top.
//
// 02-09-15 AJ  5.6-c038 Fixing some more warnings due to implicit typecasting
// 02-06-15 AJ  5.6-c037 Fixing warnings due to implicit typecasting
// 07-10-14 ADC 5.6-c015 Refactoring, partial documentation and bug fixes of XMPCommon and XMPCore C++ APIs.
//
// 10-10-12 ADC 5.5-c012 Changed internal implementation of common error notification infrastructure.
// 09-21-12 AWL 5.5-c011 Remove Transform XMP.
// 08-14-12 AWL 5.5-c009 Add XMPCore error notifications for RDF parsing errors.
// 08-08-12 AWL 5.5-c007 XMPCore error notifications for one case of XML parsing, no existing test failures.
// 08-03-12 AWL 5.5-c006 Remove defunct XMPMeta prevTkVer data member.
//
// 07-21-09 AWL 5.0-c047 Add XMPUtils::ApplyTemplate.
// 06-11-09 AWL 5.0-c034 Finish threading revamp, implement friendly reader/writer locking.
// 04-03-09 AWL 5.0-c022 Change PlainXMP to TransformXMP.
//
// 07-01-08 AWL 4.3-c060 Hide all uses of PlainXMP.
// 06-24-08 AWL 4.3-c059 [1830156] Make sure exif:UserComment is a LangAlt.
//
// 01-25-08 AWL 4.2-c037 Change all xap* namespace prefixes to xmp*.
// 11-30-07 AWL 4.2-c027 Expose XML_Node and ExpatAdapter so that XMPFiles can use them.
// 02-12-07 AWL 4.2-c010 [1482676] Remove TouchUpOneSchema from XMPMeta-Parse.cpp. Replace the loop
//              calling it with individual special cases. Some of the touchups can create or delete
//              schema, making the loop misbehave - at worst referenece beyond the end of the vector.
//
// 11-16-06 AWL 4.1-c025 [1423378] Add logic to TouchUpDataModel to repair bad AltText arrays. A
//              common problem is clients not creating the xml:lang qualifiers.
// 10-13-06 AWL 4.1-c022 [1386343] Add parsing special case to migrate xmpDM:copyright to dc:rights.
// 08-22-06 AWL 4.1-c018 [1312441,1340116] Fix another Latin-1 mistake, an infinite loop in edge
//              cases of escape checking. This is stressed and exposed by single byte parsing, but
//              could in theory occur "in the wild" with the right buffer boundaries.
// 08-21-06 AWL 4.1-c017 [1312441,1340116] Fix a bug in the Latin-1 changes: Regression testing
//              found an off-by-one error where the byte after a UTF-8 character is ignored.
// 08-16-06 AWL 4.1-c015 [1312441,1340116] Change the parsing code to tolerate ISO Latin-1 input. At
//              the same time, get rid of the ugly hackery that tries to deal with unescaped ASCII
//              control characters. Also get rid of the check and complaint about parsing into an
//              empty object. This is predominantly what people want, so clear the XMPMeta object.
//
// 03-24-06 AWL 4.0-c001 Adapt for move to ham-perforce, integrate XMPFiles, bump version to 4.
//
// 01-24-06 AWL 3.3-010 Turn off snprintf warning.
// 10-13-05 AWL 3.3-008 [1247154] Fix incorrect dc:subject array type.
//
// 06-07-05 AWL 3.2-112 Serialize empty packets nicely.
// 06-06-05 AWL 3.2-111 [0536565] Fix the compact serialization of empty structs.
// 06-03-05 AWL 3.2-109 [0467370] Parse failures must leave the XMP object empty, ready for another parse.
// 06-03-05 AWL 3.2-108 Output empty element in RDF for an empty array, e.g. <rdf:Seq/>.
// 06-02-05 AWL 3.2-105 [1016912] Use empty element form when rdf:Description has only attribute content.
// 06-01-05 AWL 3.2-105 [1110051] Add delete-existing option for SetProperty.
// 06-01-05 AWL 3.2-103 [0509601,1204017] Fix SetLocalizedText to find and set a matching "real"
//              language value when the specific_lang parameter is "x-default".
// 05-27-05 AWL 3.2-102 Move the setting of the alloc/free procs to the top of XMPMeta::Initialize.
// 05-16-05 AWL 3.2-100 Complete the deBIBification, integrate the internal and SDK source. Bump the
//              version to 3.3 and build to 100, well ahead of main's latest 3.3-009.
//
// 05-02-05 AWL 3.2-019 Add the Dynamic Media namespace, kXMP_NS_DM, "http://ns.adobe.com/xmp/1.0/DynamicMedia/".
// 04-14-05 AWL 3.2-018 Move the padding param, add overloads to simplify use of SerializeToBuffer.
// 04-13-05 AWL 3.2-017 Improve the documentation and behavior of Get/SetLocalizedText.
// 04-11-05 AWL 3.2-016 Add AdobePrivate conditionals where appropriate.
// 04-08-05 AWL 3.2-015 Undo unnecessary constant changes in XMP_Const.h - keep compatibility over
//              "perfection" so that SDK and internal code are easier to merge.
// 04-07-05 AWL 3.2-014 Fix SetLocalizedText to set the alt-text bit for empty alt array. This became
//              broken in the changes for 3.2-010 (SDK) and 3.3-003 (main). Don't throw an exception
//              if x:xmpmeta is missing and kXMP_RequireXMPMeta is used. Old code ignored that
//              candidate. This changed in the previous build's root node lookup changes.
// 04-06-05 AWL 3.2-013 [0509601] Normalize "single value" alt-text arrays. Improve the way the root
//              XML node is found and extract the previous toolkit version number.
// 04-05-05 AWL 3.2-012 [1172565] Move instance ID from rdf:about to xmpMM:InstanceID.
// 04-05-05 AWL 3.2-011 [0532345] Normalize xml:lang values so that compares are in effect case
//              insensitive as required by RFC 3066. Change parsing and serializing to force the
//              x-default item to be first.
// 04-01-05 AWL 3.2-010 Add leafOptions parameter to FindNode, used when creating new nodes.
// 03-17-05 AWL 3.2-006 Revise Plain XMP parsing and serialization for latest proposal.
// 02-16-05 AWL 3.2-005 Add first cut of Plain XMP parsing and serialization.
// 02-14-05 AWL 3.2-003 Add thread locks.
// 02-11-05 AWL 3.2-002 Add client reference counting.
// 01-28-05 AWL 3.2-001 Remove BIB.
//
// 02-02-05 AWL 3.1.1-110 [1145657] Fix XMPMeta::SerializeToBuffer to do an exact size UTF-16 or UTF-32
//              packet correctly. It was doing UTF-8 of the exact size, then converting.
// 01-26-05 AWL 3.1.1-107 [1141684] Add XMPMeta::UnregisterAssertNotify and XMPMeta::SendAssertNotify.
// 01-25-05 AWL 3.1.1-106 [1141007] Add XMPMeta::RegisterAssertNotify.
// 01-17-05 AWL 3.1.1-105 [1135773] Delete empty schema in addition to removing the pointer from the
//              tree node's child vector. This bug was introduced in the -101 changes.
// 01-07-05 AWL 3.1.1-104 [1129616] Fix SetObjectName to verify that the string is UTF-8.
// 12-14-04 AWL 3.1.1-101 [1085740] Delete empty schema nodes in CleanupParsedAliases.
// 12-14-04 AWL 3.1.1-100 [1022350,1075328] Add more namespaces and internal/external properties.
// 12-09-04 AWl 3.1.1-099 [1041438] Add parsing and serialization support for UTF-16 and UTF-32.
// 11-05-04 AWL 3.1.1-093 [1037508] Touch up exif:GPSTimeStamp values that have zero for the date.
// 11-05-04 AWL 3.1.1-092 [1106408] Add new IPTC namespace.
// 10-29-04 AWL 3.1.1-089 [1099634] Fix ParseFromBuffer to properly handle a partial escape followed
//              by another escape. E.g. "blah blah &#" then "xA;&#xA; blah blah".
// 10-20-04 AWL 3.1.1-085 [1084185] Fix XMP_InternalRef to not depend on BIBContainerBase layout.
// 10-12-04 AWL 3.1.1-084 [0616293] Add value checking to SetNodeValue, prefix checking to RegisterNamespace.
// 10-06-04 AWL 3.1.1-083 [1061778] Add lock tracing under TraceXMPLocking.
// 10-05-04 AWL 3.1.1-082 [1061778] Use recursive form of BIB mutex.
//
// 08-27-04 AWL 3.1-079 Remove overly aggressive option checks in SerializeAsRDF.
// 08-27-04 AWL 3.1-078 Add EXIF_Aux and CameraRaw namespaces.
// 08-23-04 AWL 3.1-076 Add kXMP_NS_PSAlbum as a predefined namespace.
// 08-04-04 AWL 3.1-073 [1050719] Fix handling of an empty default namespace declaration.
// 07-28-04 AWL 3.1-069 [1043286] Preflight parse buffers for ASCII controls.
// 07-26-04 AWL 3.1-068 [1046381,1046384] Fix XMPUtils::ConvertToXyz to throw instead of assert for
//              null or empty string. Fix XMPMeta::GetProperty_Xyz to make sure property is simple.
// 07-21-04 AWL 3.1-067 [1044036] Increase fudge factor for output reserve in SerializeToBuffer.
// 07-15-04 AWL 3.1-066 [1015904] Fix Get/SetLocalizedText to allow empty alt arrays.
// 07-14-04 AWL 3.1-062 [1026373] Strip old iX:changes elements during parsing.
// 07-14-04 AWL 3.1-059 [0658992] Fix SetQualifier to throw if property does not exist.
// 07-13-04 AWL 3.1-058 [1014255] Remove empty schema nodes.
// 07-13-04 AWL 3.1-057 [1006568] Check all XPath components for valid namespace prefixes.
// 07-09-04 AWL 3.1-053 [1037918] Use double escaping for ASCII controls other than tab, lf, and cr.
// 06-10-04 AWL 3.1-047 Fix more HPUX and AIX compilation errors and warnings.
// 06-04-04 AWL 3.1-046 Fix HPUX compilation errors and warnings.
//
// 05-25-04 AWL [1018426] Hide all use of cin/cout streams in #if DEBUG or equivalent.
// 05-14-04 AWL [1016811,1018220] Fix escaping bugs in serialization.
// 04-30-04 AWL Add new & delete operators that call BIBMemory functions. Change static objects that
//              require allocation to explicit pointers. Properly delete partial tree when parse
//              errors throw an exception. Clean up memory leaks.
// 04-19-04 AWL [1010249] Don't write a final newline after the packet trailer, the packet must end
//              at the '>' of the trailer.
// 04-16-04 AWL Fix major bogosity in IsAttrQualifier XMPMeta.cpp.
// 04-16-04 AWL Inflate serialization size estimate to accommodate anticipated character entities,
//              e.g. &#xA; for newlines. There can be a lot in the base 64 encoding of a thumbnail.
// 03-17-04 AWL Cleanup error exceptions, make sure all have a reasonable message. Have GetProperty
//              and friends throw an exception if the schema namespace is not registered.
// 03-04-04 AWL Add hacks to handle the DC singleton denormalization of the XMP from Acrobat 5.
// 02-19-04 AWL Add the new "transient" standard namespace.
// 02-06-04 AWL Register a couple of missing standard namespaces.
// 01-29-04 AWL Add AppendArrayItem.
// 01-26-04 AWL Revamp expanded XPath handling to work better for aliases.
// 04-24-03 AWL Initial start on the new implementation.
//
// =================================================================================================
#endif /* AdobePrivate */

#include "public/include/XMP_Environment.h" // ! This must be the first include!

#if XMP_DebugBuild
    #include <iostream>
#endif

#include "XMPCore/source/XMPCore_Impl.hpp"

#include "XMPCore/source/XMPMeta.hpp"
#include "XMPCore/source/XMPUtils.hpp"

#include "source/UnicodeInlines.incl_cpp"
#include "source/UnicodeConversions.hpp"
#include "source/ExpatAdapter.hpp"
#define  STATIC_SAFE_API
#include "source/SafeStringAPIs.h"

using namespace std;

#if XMP_WinBuild
    #pragma warning ( disable : 4533 )  // initialization of '...' is skipped by 'goto ...'
    #pragma warning ( disable : 4702 )  // unreachable code
    #pragma warning ( disable : 4800 )  // forcing value to bool 'true' or 'false' (performance warning)
    #pragma warning ( disable : 4996 )  // '...' was declared deprecated
#endif


// *** Use the XMP_PropIsXyz (Schema, Simple, Struct, Array, ...) macros
// *** Add debug codegen checks, e.g. that typical masking operations really work
// *** Change all uses of strcmp and strncmp to XMP_LitMatch and XMP_LitNMatch


// =================================================================================================
// Local Types and Constants
// =========================


// =================================================================================================
// Static Variables
// ================

#ifndef Trace_ParsingHackery
    #define Trace_ParsingHackery 0
#endif

static const char * kReplaceLatin1[128] =
    {

        // The 0x80..0x9F range is undefined in Latin-1, but is defined in Windows code page 1252.
        // The bytes 0x81, 0x8D, 0x8F, 0x90, and 0x9D are formally undefined by Windows 1252, but
        // their conversion API maps them to U+0081, etc. These are in XML's RestrictedChar set, so
        // we map them to a space.

        "\xE2\x82\xAC", " ",            "\xE2\x80\x9A", "\xC6\x92",     // 0x80 .. 0x83
        "\xE2\x80\x9E", "\xE2\x80\xA6", "\xE2\x80\xA0", "\xE2\x80\xA1", // 0x84 .. 0x87
        "\xCB\x86",     "\xE2\x80\xB0", "\xC5\xA0",     "\xE2\x80\xB9", // 0x88 .. 0x8B
        "\xC5\x92",     " ",            "\xC5\xBD",     " ",            // 0x8C .. 0x8F

        " ",            "\xE2\x80\x98", "\xE2\x80\x99", "\xE2\x80\x9C", // 0x90 .. 0x93
        "\xE2\x80\x9D", "\xE2\x80\xA2", "\xE2\x80\x93", "\xE2\x80\x94", // 0x94 .. 0x97
        "\xCB\x9C",     "\xE2\x84\xA2", "\xC5\xA1",     "\xE2\x80\xBA", // 0x98 .. 0x9B
        "\xC5\x93",     " ",            "\xC5\xBE",     "\xC5\xB8",     // 0x9C .. 0x9F

        // These are the UTF-8 forms of the official Latin-1 characters in the range 0xA0..0xFF. Not
        // too surprisingly these map to U+00A0, etc. Which is the Unicode Latin Supplement range.

        "\xC2\xA0", "\xC2\xA1", "\xC2\xA2", "\xC2\xA3", "\xC2\xA4", "\xC2\xA5", "\xC2\xA6", "\xC2\xA7", // 0xA0 .. 0xA7
        "\xC2\xA8", "\xC2\xA9", "\xC2\xAA", "\xC2\xAB", "\xC2\xAC", "\xC2\xAD", "\xC2\xAE", "\xC2\xAF", // 0xA8 .. 0xAF

        "\xC2\xB0", "\xC2\xB1", "\xC2\xB2", "\xC2\xB3", "\xC2\xB4", "\xC2\xB5", "\xC2\xB6", "\xC2\xB7", // 0xB0 .. 0xB7
        "\xC2\xB8", "\xC2\xB9", "\xC2\xBA", "\xC2\xBB", "\xC2\xBC", "\xC2\xBD", "\xC2\xBE", "\xC2\xBF", // 0xB8 .. 0xBF

        "\xC3\x80", "\xC3\x81", "\xC3\x82", "\xC3\x83", "\xC3\x84", "\xC3\x85", "\xC3\x86", "\xC3\x87", // 0xC0 .. 0xC7
        "\xC3\x88", "\xC3\x89", "\xC3\x8A", "\xC3\x8B", "\xC3\x8C", "\xC3\x8D", "\xC3\x8E", "\xC3\x8F", // 0xC8 .. 0xCF

        "\xC3\x90", "\xC3\x91", "\xC3\x92", "\xC3\x93", "\xC3\x94", "\xC3\x95", "\xC3\x96", "\xC3\x97", // 0xD0 .. 0xD7
        "\xC3\x98", "\xC3\x99", "\xC3\x9A", "\xC3\x9B", "\xC3\x9C", "\xC3\x9D", "\xC3\x9E", "\xC3\x9F", // 0xD8 .. 0xDF

        "\xC3\xA0", "\xC3\xA1", "\xC3\xA2", "\xC3\xA3", "\xC3\xA4", "\xC3\xA5", "\xC3\xA6", "\xC3\xA7", // 0xE0 .. 0xE7
        "\xC3\xA8", "\xC3\xA9", "\xC3\xAA", "\xC3\xAB", "\xC3\xAC", "\xC3\xAD", "\xC3\xAE", "\xC3\xAF", // 0xE8 .. 0xEF

        "\xC3\xB0", "\xC3\xB1", "\xC3\xB2", "\xC3\xB3", "\xC3\xB4", "\xC3\xB5", "\xC3\xB6", "\xC3\xB7", // 0xF0 .. 0xF7
        "\xC3\xB8", "\xC3\xB9", "\xC3\xBA", "\xC3\xBB", "\xC3\xBC", "\xC3\xBD", "\xC3\xBE", "\xC3\xBF", // 0xF8 .. 0xFF

    };


// =================================================================================================
// Local Utilities
// ===============


#define IsHexDigit(ch)      ( (('0' <= (ch)) && ((ch) <= '9')) || (('A' <= (ch)) && ((ch) <= 'F')) )
#define HexDigitValue(ch)   ( (((ch) - '0') < 10) ? ((ch) - '0') : ((ch) - 'A' + 10) )


// -------------------------------------------------------------------------------------------------
// PickBestRoot
// ------------
//
// Pick the first x:xmpmeta among multiple root candidates. If there aren't any, pick the first bare
// rdf:RDF if that is allowed. The returned root is the rdf:RDF child if an x:xmpmeta element was
// chosen.  The search is breadth first, so a higher level candiate is chosen over a lower level one
// that was textually earlier in the serialized XML.

static const XML_Node * PickBestRoot ( const XML_Node & xmlParent, XMP_OptionBits options )
{

#if AdobePrivate
    // Look among this parent's content for txmp:XMP_Packet or x:xmpmeta. The recursion for
    // x:xmpmeta is broader than the strictly defined choice, but gives us smaller code.
    for ( size_t childNum = 0, childLim = xmlParent.content.size(); childNum < childLim; ++childNum ) {
        const XML_Node * childNode = xmlParent.content[childNum];
        if ( childNode->kind != kElemNode ) continue;
        if ( (childNode->name == "x:xmpmeta") || (childNode->name == "x:xapmeta") ) return PickBestRoot ( *childNode, 0 );
    }
#else
    // Look among this parent's content for x:xmpmeta. The recursion for x:xmpmeta is broader than
    // the strictly defined choice, but gives us smaller code.
    for ( size_t childNum = 0, childLim = xmlParent.content.size(); childNum < childLim; ++childNum ) {
        const XML_Node * childNode = xmlParent.content[childNum];
        if ( childNode->kind != kElemNode ) continue;
        if ( (childNode->name == "x:xmpmeta") || (childNode->name == "x:xapmeta") ) return PickBestRoot ( *childNode, 0 );
    }
#endif

    // Look among this parent's content for a bare rdf:RDF if that is allowed.
    if ( ! (options & kXMP_RequireXMPMeta) ) {
        for ( size_t childNum = 0, childLim = xmlParent.content.size(); childNum < childLim; ++childNum ) {
            const XML_Node * childNode = xmlParent.content[childNum];
            if ( childNode->kind != kElemNode ) continue;
            if ( childNode->name == "rdf:RDF" ) return childNode;
        }
    }

    // Recurse into the content.
    for ( size_t childNum = 0, childLim = xmlParent.content.size(); childNum < childLim; ++childNum ) {
        const XML_Node * foundRoot = PickBestRoot ( *xmlParent.content[childNum], options );
        if ( foundRoot != 0 ) return foundRoot;
    }

    return 0;

}   // PickBestRoot

// -------------------------------------------------------------------------------------------------
// FindRootNode
// ------------
//
// Find the XML node that is the root of the XMP data tree. Generally this will be an outer node,
// but it could be anywhere if a general XML document is parsed (e.g. SVG). The XML parser counted
// all possible root nodes, and kept a pointer to the last one. If there is more than one possible
// root use PickBestRoot to choose among them.
//
// If there is a root node, try to extract the version of the previous XMP toolkit.

static const XML_Node * FindRootNode ( const XMLParserAdapter & xmlParser, XMP_OptionBits options )
{
    const XML_Node * rootNode = xmlParser.rootNode;

    if ( xmlParser.rootCount > 1 ) rootNode = PickBestRoot ( xmlParser.tree, options );
    if ( rootNode == 0 ) return 0;

        XMP_Assert ( rootNode->name == "rdf:RDF" );

        if ( (options & kXMP_RequireXMPMeta) &&
             ((rootNode->parent == 0) ||
              ((rootNode->parent->name != "x:xmpmeta") && (rootNode->parent->name != "x:xapmeta"))) ) return 0;

    return rootNode;

}   // FindRootNode

// -------------------------------------------------------------------------------------------------
// NormalizeDCArrays
// -----------------
//
// Undo the denormalization performed by the XMP used in Acrobat 5. If a Dublin Core array had only
// one item, it was serialized as a simple property. The xml:lang attribute was dropped from an
// alt-text item if the language was x-default.

// *** This depends on the dc: namespace prefix.

void
NormalizeDCArrays ( XMP_Node * xmpTree )
{
    XMP_Node * dcSchema = FindSchemaNode ( xmpTree, kXMP_NS_DC, kXMP_ExistingOnly );
    if ( dcSchema == 0 ) return;

    for ( size_t propNum = 0, propLimit = dcSchema->children.size(); propNum < propLimit; ++propNum ) {
        XMP_Node *     currProp  = dcSchema->children[propNum];
        XMP_OptionBits arrayForm = 0;

        if ( ! XMP_PropIsSimple ( currProp->options ) ) continue;   // Nothing to do if not simple.

        if ( (currProp->name == "dc:creator" )     ||   // See if it is supposed to be an array.
             (currProp->name == "dc:date" ) ) {         // *** Think about an array of char* and a loop.
            arrayForm = kXMP_PropArrayIsOrdered;
        } else if (
             (currProp->name == "dc:description" ) ||
             (currProp->name == "dc:rights" )      ||
             (currProp->name == "dc:title" ) ) {
            arrayForm = kXMP_PropArrayIsAltText;
        } else if (
             (currProp->name == "dc:contributor" ) ||
             (currProp->name == "dc:language" )    ||
             (currProp->name == "dc:publisher" )   ||
             (currProp->name == "dc:relation" )    ||
             (currProp->name == "dc:subject" )     ||
             (currProp->name == "dc:type" ) ) {
            arrayForm = kXMP_PropValueIsArray;
        }
        if ( arrayForm == 0 ) continue; // Nothing to do if it isn't supposed to be an array.

        arrayForm = VerifySetOptions ( arrayForm, 0 );  // Set the implicit array bits.
        XMP_Node * newArray = new XMP_Node ( dcSchema, currProp->name.c_str(), arrayForm );
        dcSchema->children[propNum] = newArray;

        if ( currProp->value.empty() ) {    // Don't add an empty item, leave the array empty.

            delete ( currProp );

        } else {

            newArray->children.push_back ( currProp );
            currProp->parent = newArray;
            currProp->name = kXMP_ArrayItemName;

            if ( XMP_ArrayIsAltText ( arrayForm ) && (! (currProp->options & kXMP_PropHasLang)) ) {
                XMP_Node * newLang = new XMP_Node ( currProp, "xml:lang", "x-default", kXMP_PropIsQualifier );
                currProp->options |= (kXMP_PropHasQualifiers | kXMP_PropHasLang);
                if ( currProp->qualifiers.empty() ) {   // *** Need a util?
                    currProp->qualifiers.push_back ( newLang );
                } else {
                    currProp->qualifiers.insert ( currProp->qualifiers.begin(), newLang );
                }
            }

        }

    }

}   // NormalizeDCArrays


// -------------------------------------------------------------------------------------------------
// CompareAliasedSubtrees
// ----------------------

// *** Change to do some alias-specific setup, then use CompareSubtrees. One special case for
// *** aliases is a simple to x-default alias, the options and qualifiers obviously differ.

static void
CompareAliasedSubtrees ( XMP_Node * aliasNode, XMP_Node * baseNode,
                         XMPMeta::ErrorCallbackInfo & errorCallback, bool outerCall = true )
{
    // ! The outermost call is special. The names almost certainly differ. The qualifiers (and
    // ! hence options) will differ for an alias to the x-default item of a langAlt array.

    if ( (aliasNode->value != baseNode->value) ||
         (aliasNode->children.size() != baseNode->children.size()) ) {
        // Keep things simple for now. Aliases are virtually unused, so this is very unlikely to
        // happen. Recovery can be added later if it becomes important.
        XMP_Error error(kXMPErr_BadXMP, "Mismatch between alias and base nodes");
        errorCallback.NotifyClient ( kXMPErrSev_OperationFatal, error );
    }

    if ( ! outerCall ) {
        if ( (aliasNode->name != baseNode->name) ||
             (aliasNode->options != baseNode->options) ||
             (aliasNode->qualifiers.size() != baseNode->qualifiers.size()) ) {
            // Keep things simple for now. Aliases are virtually unused, so this is very unlikely to
            // happen. Recovery can be added later if it becomes important.
            XMP_Error error(kXMPErr_BadXMP, "Mismatch between alias and base nodes");
            errorCallback.NotifyClient ( kXMPErrSev_OperationFatal, error );
        }
    }

    for ( size_t childNum = 0, childLim = aliasNode->children.size(); childNum < childLim; ++childNum ) {
        XMP_Node * aliasChild = aliasNode->children[childNum];
        XMP_Node * baseChild  = baseNode->children[childNum];
        CompareAliasedSubtrees ( aliasChild, baseChild, errorCallback, false );
    }

    for ( size_t qualNum = 0, qualLim = aliasNode->qualifiers.size(); qualNum < qualLim; ++qualNum ) {
        XMP_Node * aliasQual = aliasNode->qualifiers[qualNum];
        XMP_Node * baseQual  = baseNode->qualifiers[qualNum];
        CompareAliasedSubtrees ( aliasQual, baseQual, errorCallback, false );
    }

}   // CompareAliasedSubtrees


// -------------------------------------------------------------------------------------------------
// TransplantArrayItemAlias
// ------------------------

static void
TransplantArrayItemAlias ( XMP_Node * oldParent, size_t oldNum, XMP_Node * newParent,
                           XMPMeta::ErrorCallbackInfo & errorCallback )
{
    XMP_Node * childNode = oldParent->children[oldNum];

    if ( newParent->options & kXMP_PropArrayIsAltText ) {
        if ( childNode->options & kXMP_PropHasLang ) {
            // Keep things simple for now. Aliases are virtually unused, so this is very unlikely to
            // happen. Recovery can be added later if it becomes important.
            XMP_Error error(kXMPErr_BadXMP, "Alias to x-default already has a language qualifier");
            errorCallback.NotifyClient ( kXMPErrSev_OperationFatal, error );    // *** Allow x-default.
        }
        childNode->options |= (kXMP_PropHasQualifiers | kXMP_PropHasLang);
        XMP_Node * langQual = new XMP_Node ( childNode, "xml:lang", "x-default", kXMP_PropIsQualifier );    // *** AddLangQual util?
        if ( childNode->qualifiers.empty() ) {
            childNode->qualifiers.push_back ( langQual );
        } else {
            childNode->qualifiers.insert ( childNode->qualifiers.begin(), langQual );
        }
    }

    oldParent->children.erase ( oldParent->children.begin() + oldNum );
    childNode->name = kXMP_ArrayItemName;
    childNode->parent = newParent;
    if ( newParent->children.empty() ) {
        newParent->children.push_back ( childNode );
    } else {
        newParent->children.insert ( newParent->children.begin(), childNode );
    }

}   // TransplantArrayItemAlias


// -------------------------------------------------------------------------------------------------
// TransplantNamedAlias
// --------------------

static void
TransplantNamedAlias ( XMP_Node * oldParent, size_t oldNum, XMP_Node * newParent, XMP_VarString & newName )
{
    XMP_Node * childNode = oldParent->children[oldNum];

    oldParent->children.erase ( oldParent->children.begin() + oldNum );
    childNode->name = newName;
    childNode->parent = newParent;
    newParent->children.push_back ( childNode );

}   // TransplantNamedAlias


// -------------------------------------------------------------------------------------------------
// MoveExplicitAliases
// -------------------

void
MoveExplicitAliases ( XMP_Node * tree, XMP_OptionBits parseOptions, XMPMeta::ErrorCallbackInfo & errorCallback )
{
    tree->options ^= kXMP_PropHasAliases;
    const bool strictAliasing = ((parseOptions & kXMP_StrictAliasing) != 0);

    // Visit all of the top level nodes looking for aliases. If there is no base, transplant the
    // alias subtree. If there is a base and strict aliasing is on, make sure the alias and base
    // subtrees match.

    // ! Use "while" loops not "for" loops since both the schema and property loops can remove the
    // ! current item from the vector being traversed. And don't increment the counter for a delete.

    size_t schemaNum = 0;
    while ( schemaNum < tree->children.size() ) {
        XMP_Node * currSchema = tree->children[schemaNum];

        size_t propNum = 0;
        while ( propNum < currSchema->children.size() ) {
            XMP_Node * currProp = currSchema->children[propNum];
            if ( ! (currProp->options & kXMP_PropIsAlias) ) {
                ++propNum;
                continue;
            }
            currProp->options ^= kXMP_PropIsAlias;

            // Find the base path, look for the base schema and root node.

            XMP_AliasMapPos aliasPos = sRegisteredAliasMap->find ( currProp->name );
            XMP_Assert ( aliasPos != sRegisteredAliasMap->end() );
            XMP_ExpandedXPath & basePath = aliasPos->second;
            XMP_OptionBits arrayOptions = (basePath[kRootPropStep].options & kXMP_PropArrayFormMask);

            XMP_Node * baseSchema = FindSchemaNode ( tree, basePath[kSchemaStep].step.c_str(), kXMP_CreateNodes );
            if ( baseSchema->options & kXMP_NewImplicitNode ) baseSchema->options ^= kXMP_NewImplicitNode;
            XMP_Node * baseNode = FindChildNode ( baseSchema, basePath[kRootPropStep].step.c_str(), kXMP_ExistingOnly );

            if ( baseNode == 0 ) {

                if ( basePath.size() == 2 ) {
                    // A top-to-top alias, transplant the property.
                    TransplantNamedAlias ( currSchema, propNum, baseSchema, basePath[kRootPropStep].step );
                } else {
                    // An alias to an array item, create the array and transplant the property.
                    baseNode = new XMP_Node ( baseSchema, basePath[kRootPropStep].step.c_str(), arrayOptions );
                    baseSchema->children.push_back ( baseNode );
                    TransplantArrayItemAlias ( currSchema, propNum, baseNode, errorCallback );
                }

            } else if ( basePath.size() == 2 ) {

                // The base node does exist and this is a top-to-top alias. Check for conflicts if
                // strict aliasing is on. Remove and delete the alias subtree.
                if ( strictAliasing ) CompareAliasedSubtrees ( currProp, baseNode, errorCallback );
                currSchema->children.erase ( currSchema->children.begin() + propNum );
                delete currProp;

            } else {

                // This is an alias to an array item and the array exists. Look for the aliased item.
                // Then transplant or check & delete as appropriate.

                XMP_Node * itemNode = 0;
                if ( arrayOptions & kXMP_PropArrayIsAltText ) {
                    XMP_Index xdIndex = LookupLangItem ( baseNode, *xdefaultName );
                    if ( xdIndex != -1 ) itemNode = baseNode->children[xdIndex];
                } else if ( ! baseNode->children.empty() ) {
                    itemNode = baseNode->children[0];
                }

                if ( itemNode == 0 ) {
                    TransplantArrayItemAlias ( currSchema, propNum, baseNode, errorCallback );
                } else {
                    if ( strictAliasing ) CompareAliasedSubtrees ( currProp, itemNode, errorCallback );
                    currSchema->children.erase ( currSchema->children.begin() + propNum );
                    delete currProp;
                }

            }

        }   // Property loop

        // Increment the counter or remove an empty schema node.
        if ( currSchema->children.size() > 0 ) {
            ++schemaNum;
        } else {
            delete tree->children[schemaNum];   // ! Delete the schema node itself.
            tree->children.erase ( tree->children.begin() + schemaNum );
        }

    }   // Schema loop

}   // MoveExplicitAliases


// -------------------------------------------------------------------------------------------------
// FixGPSTimeStamp
// ---------------

static void
FixGPSTimeStamp ( XMP_Node * exifSchema, XMP_Node * gpsDateTime )
{
    XMP_DateTime binGPSStamp;
    try {
        XMPUtils::ConvertToDate ( gpsDateTime->value.c_str(), &binGPSStamp );
    } catch ( ... ) {
        return; // Don't let a bad date stop other things.
    }
    if ( (binGPSStamp.year != 0) || (binGPSStamp.month != 0) || (binGPSStamp.day != 0) ) return;

    XMP_Node * otherDate = FindChildNode ( exifSchema, "exif:DateTimeOriginal", kXMP_ExistingOnly );
    if ( otherDate == 0 ) otherDate = FindChildNode ( exifSchema, "exif:DateTimeDigitized", kXMP_ExistingOnly );
    if ( otherDate == 0 ) return;

    XMP_DateTime binOtherDate;
    try {
        XMPUtils::ConvertToDate ( otherDate->value.c_str(), &binOtherDate );
    } catch ( ... ) {
        return; // Don't let a bad date stop other things.
    }

    binGPSStamp.year  = binOtherDate.year;
    binGPSStamp.month = binOtherDate.month;
    binGPSStamp.day   = binOtherDate.day;

    XMPUtils::ConvertFromDate ( binGPSStamp, &gpsDateTime->value );

}   // FixGPSTimeStamp


// -------------------------------------------------------------------------------------------------
// MigrateAudioCopyright
// ---------------------
//
// The initial support for WAV files mapped a legacy ID3 audio copyright into a new xmpDM:copyright
// property. This is special case code to migrate that into dc:rights['x-default']. The rules:
//
//   1. If there is no dc:rights array, or an empty array -
//      Create one with dc:rights['x-default'] set from double linefeed and xmpDM:copyright.
//
//   2. If there is a dc:rights array but it has no x-default item -
//      Create an x-default item as a copy of the first item then apply rule #3.
//
//   3. If there is a dc:rights array with an x-default item, look for a double linefeed in the value.
//      A. If no double linefeed, compare the x-default value to the xmpDM:copyright value.
//         A1. If they match then leave the x-default value alone.
//         A2. Otherwise, append a double linefeed and the xmpDM:copyright value to the x-default value.
//      B. If there is a double linefeed, compare the trailing text to the xmpDM:copyright value.
//         B1. If they match then leave the x-default value alone.
//         B2. Otherwise, replace the trailing x-default text with the xmpDM:copyright value.
//
//   4. In all cases, delete the xmpDM:copyright property.

static void
MigrateAudioCopyright ( XMPMeta * xmp, XMP_Node * dmCopyright )
{

    try {

        std::string & dmValue = dmCopyright->value;
        static const char * kDoubleLF = "\xA\xA";

        XMP_Node * dcSchema = FindSchemaNode ( &xmp->tree, kXMP_NS_DC, kXMP_CreateNodes );
        XMP_Node * dcRightsArray = FindChildNode ( dcSchema, "dc:rights", kXMP_ExistingOnly );

        if ( (dcRightsArray == 0) || dcRightsArray->children.empty() ) {

            // 1. No dc:rights array, create from double linefeed and xmpDM:copyright.
            dmValue.insert ( 0, kDoubleLF );
            xmp->SetLocalizedText ( kXMP_NS_DC, "rights", "", "x-default",  dmValue.c_str(), 0 );

        } else {

            std::string xdefaultStr ( "x-default" );

            XMP_Index xdIndex = LookupLangItem ( dcRightsArray, xdefaultStr );

            if ( xdIndex < 0 ) {
                // 2. No x-default item, create from the first item.
                XMP_StringPtr firstValue = dcRightsArray->children[0]->value.c_str();
                xmp->SetLocalizedText ( kXMP_NS_DC, "rights", "", "x-default",  firstValue, 0 );
                xdIndex = LookupLangItem ( dcRightsArray, xdefaultStr );
            }

            // 3. Look for a double linefeed in the x-default value.
            XMP_Assert ( xdIndex == 0 );
            std::string & defaultValue = dcRightsArray->children[xdIndex]->value;
            XMP_Index lfPos = static_cast<XMP_Index>( defaultValue.find ( kDoubleLF ));

            if ( lfPos < 0 ) {

                // 3A. No double LF, compare whole values.
                if ( dmValue != defaultValue ) {
                    // 3A2. Append the xmpDM:copyright to the x-default item.
                    defaultValue += kDoubleLF;
                    defaultValue += dmValue;
                }

            } else {

                // 3B. Has double LF, compare the tail.
                if ( defaultValue.compare ( lfPos+2, std::string::npos, dmValue ) != 0 ) {
                    // 3B2. Replace the x-default tail.
                    defaultValue.replace ( lfPos+2, std::string::npos, dmValue );
                }

            }

        }

        // 4. Get rid of the xmpDM:copyright.
        xmp->DeleteProperty ( kXMP_NS_DM, "copyright" );

    } catch ( ... ) {
        // Don't let failures (like a bad dc:rights form) stop other cleanup.
    }

}   // MigrateAudioCopyright


// -------------------------------------------------------------------------------------------------
// RepairAltText
// -------------
//
// Make sure that the array is well-formed AltText. Each item must be simple and have an xml:lang
// qualifier. If repairs are needed, keep simple non-empty items by adding the xml:lang.

static void
RepairAltText ( XMP_Node & tree, XMP_StringPtr schemaNS, XMP_StringPtr arrayName )
{
    XMP_Node * schemaNode = FindSchemaNode ( &tree, schemaNS, kXMP_ExistingOnly );
    if ( schemaNode == 0 ) return;

    XMP_Node * arrayNode = FindChildNode ( schemaNode, arrayName, kXMP_ExistingOnly );
    if ( (arrayNode == 0) || XMP_ArrayIsAltText ( arrayNode->options ) ) return;    // Already OK.

    if ( ! XMP_PropIsArray ( arrayNode->options ) ) return; // ! Not even an array, leave it alone.
    // *** Should probably change simple values to LangAlt with 'x-default' item.

    arrayNode->options |= (kXMP_PropArrayIsOrdered | kXMP_PropArrayIsAlternate | kXMP_PropArrayIsAltText);

    for ( int i = static_cast<int>( arrayNode->children.size()-1 ); i >= 0; --i ) { // ! Need a signed index type.

        XMP_Node * currChild = arrayNode->children[i];

        if ( ! XMP_PropIsSimple ( currChild->options ) ) {

            // Delete non-simple children.
            delete ( currChild );
            arrayNode->children.erase ( arrayNode->children.begin() + i );

        } else if ( ! XMP_PropHasLang ( currChild->options ) ) {

            if ( currChild->value.empty() ) {

                // Delete empty valued children that have no xml:lang.
                delete ( currChild );
                arrayNode->children.erase ( arrayNode->children.begin() + i );

            } else {

                // Add an xml:lang qualifier with the value "x-repair".
                XMP_Node * repairLang = new XMP_Node ( currChild, "xml:lang", "x-repair", kXMP_PropIsQualifier );
                if ( currChild->qualifiers.empty() ) {
                    currChild->qualifiers.push_back ( repairLang );
                } else {
                    currChild->qualifiers.insert ( currChild->qualifiers.begin(), repairLang );
                }
                currChild->options |= (kXMP_PropHasQualifiers | kXMP_PropHasLang);

            }

        }

    }

}   // RepairAltText


// -------------------------------------------------------------------------------------------------
// TouchUpDataModel
// ----------------

void
TouchUpDataModel ( XMPMeta * xmp, XMPMeta::ErrorCallbackInfo & errorCallback )
{
    XMP_Node & tree = xmp->tree;

    // Do special case touch ups for certain schema.

    XMP_Node * currSchema = 0;

    currSchema = FindSchemaNode ( &tree, kXMP_NS_EXIF, kXMP_ExistingOnly );
    if ( currSchema != 0 ) {

        // Do a special case fix for exif:GPSTimeStamp.
        XMP_Node * gpsDateTime = FindChildNode ( currSchema, "exif:GPSTimeStamp", kXMP_ExistingOnly );
        if ( gpsDateTime != 0 ) FixGPSTimeStamp ( currSchema, gpsDateTime );

        // *** Should probably have RepairAltText change simple values to LangAlt with 'x-default' item.
        // *** For now just do this for exif:UserComment, the one case we know about, late in cycle fix.
        XMP_Node * userComment = FindChildNode ( currSchema, "exif:UserComment", kXMP_ExistingOnly );
        if ( (userComment != 0) && XMP_PropIsSimple ( userComment->options ) ) {
            XMP_Node * newChild = new XMP_Node ( userComment, kXMP_ArrayItemName,
                                                 userComment->value.c_str(), userComment->options );
            newChild->qualifiers.swap ( userComment->qualifiers );
            if ( ! XMP_PropHasLang ( newChild->options ) ) {
                XMP_Node * langQual = new XMP_Node ( newChild, "xml:lang", "x-default", kXMP_PropIsQualifier );
                newChild->qualifiers.insert ( newChild->qualifiers.begin(), langQual );
                newChild->options |= (kXMP_PropHasQualifiers | kXMP_PropHasLang);
            }
            userComment->value.erase();
            userComment->options = kXMP_PropArrayFormMask;  // ! Happens to have all the right bits.
            userComment->children.push_back ( newChild );
        }

    }

    currSchema = FindSchemaNode ( &tree, kXMP_NS_DM, kXMP_ExistingOnly );
    if ( currSchema != 0 ) {
        // Do a special case migration of xmpDM:copyright to dc:rights['x-default']. Do this before
        // the dc: touch up since it can affect the dc: schema.
        XMP_Node * dmCopyright = FindChildNode ( currSchema, "xmpDM:copyright", kXMP_ExistingOnly );
        if ( dmCopyright != 0 ) MigrateAudioCopyright ( xmp, dmCopyright );
    }

    currSchema = FindSchemaNode ( &tree, kXMP_NS_DC, kXMP_ExistingOnly );
    if ( currSchema != 0 ) {
        // Do a special case fix for dc:subject, make sure it is an unordered array.
        XMP_Node * dcSubject = FindChildNode ( currSchema, "dc:subject", kXMP_ExistingOnly );
        if ( dcSubject != 0 ) {
            XMP_OptionBits keepMask = ~(kXMP_PropArrayIsOrdered | kXMP_PropArrayIsAlternate | kXMP_PropArrayIsAltText);
            dcSubject->options &= keepMask; // Make sure any ordered array bits are clear.
        }
    }

    // Fix any broken AltText arrays that we know about.

    RepairAltText ( tree, kXMP_NS_DC, "dc:description" );   // ! Note inclusion of prefixes for direct node lookup!
    RepairAltText ( tree, kXMP_NS_DC, "dc:rights" );
    RepairAltText ( tree, kXMP_NS_DC, "dc:title" );
    RepairAltText ( tree, kXMP_NS_XMP_Rights, "xmpRights:UsageTerms" );
    RepairAltText ( tree, kXMP_NS_EXIF, "exif:UserComment" );

    // Tweak old XMP: Move an instance ID from rdf:about to the xmpMM:InstanceID property. An old
    // instance ID usually looks like "uuid:bac965c4-9d87-11d9-9a30-000d936b79c4", plus InDesign
    // 3.0 wrote them like "bac965c4-9d87-11d9-9a30-000d936b79c4". If the name looks like a UUID
    // simply move it to xmpMM:InstanceID, don't worry about any existing xmpMM:InstanceID. Both
    // will only be present when a newer file with the xmpMM:InstanceID property is updated by an
    // old app that uses rdf:about.

    if ( ! tree.name.empty() ) {

        bool nameIsUUID = false;
        XMP_StringPtr nameStr = tree.name.c_str();

        if ( XMP_LitNMatch ( nameStr, "uuid:", 5 ) ) {

            nameIsUUID = true;

        } else if ( tree.name.size() == 36 ) {

            nameIsUUID = true;  // ! Assume true, we'll set it to false below if not.
            for ( int i = 0;  i < 36; ++i ) {
                char ch = nameStr[i];
                if ( ch == '-' ) {
                    if ( (i == 8) || (i == 13) || (i == 18) || (i == 23) ) continue;
                    nameIsUUID = false;
                    break;
                } else {
                    if ( (('0' <= ch) && (ch <= '9')) || (('a' <= ch) && (ch <= 'z')) ) continue;
                    nameIsUUID = false;
                    break;
                }
            }

        }

        if ( nameIsUUID ) {

            XMP_ExpandedXPath expPath;
            ExpandXPath ( kXMP_NS_XMP_MM, "InstanceID", &expPath );
            XMP_Node * idNode = FindNode ( &tree, expPath, kXMP_CreateNodes, 0 );
            if ( idNode == 0 ) XMP_Throw ( "Failure creating xmpMM:InstanceID", kXMPErr_InternalFailure );

            idNode->options = 0;    // Clobber any existing xmpMM:InstanceID.
            idNode->value = tree.name;
            idNode->RemoveChildren();
            idNode->RemoveQualifiers();

            tree.name.erase();

        }

    }

}   // TouchUpDataModel


// -------------------------------------------------------------------------------------------------
// DetermineInputEncoding
// ----------------------
//
// Try to determine the character encoding, making a guess if the input is too short. We make some
// simplifying assumtions: the first character must be U+FEFF or ASCII, U+0000 is not allowed. The
// XML 1.1 spec is even more strict, UTF-16 XML documents must begin with U+FEFF, and the first
// "real" character must be '<'. Ignoring the XML declaration, the first XML character could be '<',
// space, tab, CR, or LF.
//
// The possible input sequences are:
//
//   Cases with U+FEFF
//      EF BB BF -- - UTF-8
//      FE FF -- -- - Big endian UTF-16
//      00 00 FE FF - Big endian UTF 32
//      FF FE 00 00 - Little endian UTF-32
//      FF FE -- -- - Little endian UTF-16
//
//   Cases with ASCII
//      nn mm -- -- - UTF-8 -
//      00 00 00 nn - Big endian UTF-32
//      00 nn -- -- - Big endian UTF-16
//      nn 00 00 00 - Little endian UTF-32
//      nn 00 -- -- - Little endian UTF-16
//
// ! We don't check for full patterns, or for errors. We just check enough to determine what the
// ! only possible (or reasonable) case would be.

static XMP_OptionBits
DetermineInputEncoding ( const XMP_Uns8 * buffer, size_t length )
{
    if ( length < 2 ) return kXMP_EncodeUTF8;

    XMP_Uns8 * uniChar = (XMP_Uns8*)buffer; // ! Make sure comparisons are unsigned.

    if ( uniChar[0] == 0 ) {

        // These cases are:
        //   00 nn -- -- - Big endian UTF-16
        //   00 00 00 nn - Big endian UTF-32
        //   00 00 FE FF - Big endian UTF 32

        if ( (length < 4) || (uniChar[1] != 0) ) return kXMP_EncodeUTF16Big;
        return kXMP_EncodeUTF32Big;

    } else if ( uniChar[0] < 0x80 ) {

        // These cases are:
        //   nn mm -- -- - UTF-8, includes EF BB BF case
        //   nn 00 00 00 - Little endian UTF-32
        //   nn 00 -- -- - Little endian UTF-16

        if ( uniChar[1] != 0 )  return kXMP_EncodeUTF8;
        if ( (length < 4) || (uniChar[2] != 0) ) return kXMP_EncodeUTF16Little;
        return kXMP_EncodeUTF32Little;

    } else {

        // These cases are:
        //   EF BB BF -- - UTF-8
        //   FE FF -- -- - Big endian UTF-16
        //   FF FE 00 00 - Little endian UTF-32
        //   FF FE -- -- - Little endian UTF-16

        if ( uniChar[0] == 0xEF ) return kXMP_EncodeUTF8;
        if ( uniChar[0] == 0xFE ) return kXMP_EncodeUTF16Big;
        if ( (length < 4) || (uniChar[2] != 0) ) return kXMP_EncodeUTF16Little;
        return kXMP_EncodeUTF32Little;

    }

}   // DetermineInputEncoding


// -------------------------------------------------------------------------------------------------
// CountUTF8
// ---------
//
// Look for a valid multi-byte UTF-8 sequence and return its length. Returns 0 for an invalid UTF-8
// sequence. Returns a negative value for a partial valid sequence at the end of the buffer.
//
// The checking is not strict. We simply count the number of high order 1 bits in the first byte,
// then look for n-1 following bytes whose high order 2 bits are 1 and 0. We do not check for a
// minimal length representation of the codepoint, or that the codepoint is defined by Unicode.

static int
CountUTF8 ( const XMP_Uns8 * charStart, const XMP_Uns8 * bufEnd )
{
    XMP_Assert ( charStart < bufEnd );      // Catch this in debug builds.
    if ( charStart >= bufEnd ) return 0;    // Don't run-on in release builds.
    if ( (*charStart & 0xC0) != 0xC0 ) return 0;    // Must have at least 2 high bits set.

    int byteCount = 2;
    XMP_Uns8 firstByte = *charStart;
    for ( firstByte = firstByte << 2; (firstByte & 0x80) != 0; firstByte = firstByte << 1 ) ++byteCount;

    if ( (charStart + byteCount) > bufEnd ) return -byteCount;

    for ( int i = 1; i < byteCount; ++i ) {
        if ( (charStart[i] & 0xC0) != 0x80 ) return 0;
    }

    return byteCount;

}   // CountUTF8


// -------------------------------------------------------------------------------------------------
// CountControlEscape
// ------------------
//
// Look for a numeric escape sequence for a "prohibited" ASCII control character. These are 0x7F,
// and the range 0x00..0x1F except for tab/LF/CR. Return 0 if this is definitely not a numeric
// escape, the length of the escape if found, or a negative value for a partial escape.

static int
CountControlEscape ( const XMP_Uns8 * escStart, const XMP_Uns8 * bufEnd )
{
    XMP_Assert ( escStart < bufEnd );   // Catch this in debug builds.
    if ( escStart >= bufEnd ) return 0; // Don't run-on in release builds.
    XMP_Assert ( *escStart == '&' );

    size_t tailLen = bufEnd - escStart;
    if ( tailLen < 5 ) return -1;   // Don't need a more thorough check, we'll catch it on the next pass.

    if ( strncmp ( (char*)escStart, "&#x", 3 ) != 0 ) return 0;

    XMP_Uns8 escValue = 0;
    const XMP_Uns8 * escPos = escStart + 3;

    if ( ('0' <= *escPos) && (*escPos <= '9') ) {
        escValue = *escPos - '0';
        ++escPos;
    } else if ( ('A' <= *escPos) && (*escPos <= 'F') ) {
        escValue = *escPos - 'A' + 10;
        ++escPos;
    } else if ( ('a' <= *escPos) && (*escPos <= 'f') ) {
        escValue = *escPos - 'a' + 10;
        ++escPos;
    }

    if ( ('0' <= *escPos) && (*escPos <= '9') ) {
        escValue = (escValue << 4) + (*escPos - '0');
        ++escPos;
    } else if ( ('A' <= *escPos) && (*escPos <= 'F') ) {
        escValue = (escValue << 4) + (*escPos - 'A' + 10);
        ++escPos;
    } else if ( ('a' <= *escPos) && (*escPos <= 'f') ) {
        escValue = (escValue << 4) + (*escPos - 'a' + 10);
        ++escPos;
    }

    if ( escPos == bufEnd ) return -1;  // Partial escape.
    if ( *escPos != ';' ) return 0;

    size_t escLen = escPos - escStart + 1;
    if ( escLen < 5 ) return 0; // ! Catch "&#x;".

    if ( (escValue == kTab) || (escValue == kLF) || (escValue == kCR) ) return 0;   // An allowed escape.

    return static_cast<int>(escLen);    // Found a full "prohibited" numeric escape.

}   // CountControlEscape


// -------------------------------------------------------------------------------------------------
// ProcessUTF8Portion
// ------------------
//
// Early versions of the XMP spec mentioned allowing ISO Latin-1 input. There are also problems with
// some clients placing ASCII control characters within XMP values. This is an XML problem, the XML
// spec only allows tab (0x09), LF (0x0A), and CR (0x0D) from the 0x00..0x1F range. As a concession
// to this we scan 8-bit input for byte sequences that are not valid UTF-8 or in the 0x00..0x1F
// range and replace each byte as follows:
//   0x00..0x1F - Replace with a space, except for tab, CR, and LF.
//   0x7F       - Replace with a space. This is ASCII Delete, not allowed by ISO Latin-1.
//   0x80..0x9F - Replace with the UTF-8 for a corresponding Unicode character.
//   0xA0..0XFF - Replace with the UTF-8 for a corresponding Unicode character.
//
// The 0x80..0x9F range is not defined by Latin-1. But the Windows 1252 code page defines these and
// is otherwise the same as Latin-1.
//
// For at least historical compatibility reasons we also find and replace singly escaped ASCII
// control characters. The Expat parser we're using does not allow numeric escapes like "&#x10;".
// The XML spec is clear that raw controls are not allowed (in the RestrictedChar set), but it isn't
// as clear about numeric escapes for them. At any rate, Expat complains, so we treat the numeric
// escapes like raw characters and replace them with a space.
//
// We check for 1 or 2 hex digits ("&#x9;" or "&#x09;") and upper or lower case ("&#xA;" or "&#xa;").
// The full escape sequence is 5 or 6 bytes.

static size_t
ProcessUTF8Portion ( XMLParserAdapter * xmlParser,
                     const XMP_Uns8 *   buffer,
                     size_t             length,
                     bool               last )
{
    const XMP_Uns8 * bufEnd = buffer + length;

    const XMP_Uns8 * spanStart = buffer;
    const XMP_Uns8 * spanEnd;

    for ( spanEnd = spanStart; spanEnd < bufEnd; ++spanEnd ) {

        if ( (0x20 <= *spanEnd) && (*spanEnd <= 0x7E) && (*spanEnd != '&') ) continue;  // A regular ASCII character.

        if ( *spanEnd >= 0x80 ) {

            // See if this is a multi-byte UTF-8 sequence, or a Latin-1 character to replace.

            int uniLen = CountUTF8 ( spanEnd, bufEnd );

            if ( uniLen > 0 ) {

                // A valid UTF-8 character, keep it as-is.
                spanEnd += uniLen - 1;  // ! The loop increment will put back the +1.

            } else if ( (uniLen < 0) && (! last) ) {

                // Have a partial UTF-8 character at the end of the buffer and more input coming.
                xmlParser->ParseBuffer ( spanStart, (spanEnd - spanStart), false );
                return (spanEnd - buffer);

            } else {

                // Not a valid UTF-8 sequence. Replace the first byte with the Latin-1 equivalent.
                xmlParser->ParseBuffer ( spanStart, (spanEnd - spanStart), false );
                const char * replacement = kReplaceLatin1 [ *spanEnd - 0x80 ];
                xmlParser->ParseBuffer(replacement, strnlen_safe(replacement, Max_XMP_Uns32), false);
                spanStart = spanEnd + 1;    // ! The loop increment will do "spanEnd = spanStart".

            }

        } else if ( (*spanEnd < 0x20) || (*spanEnd == 0x7F) ) {

            // Replace ASCII controls other than tab, LF, and CR with a space.

            if ( (*spanEnd == kTab) || (*spanEnd == kLF) || (*spanEnd == kCR) ) continue;

            xmlParser->ParseBuffer ( spanStart, (spanEnd - spanStart), false );
            xmlParser->ParseBuffer ( " ", 1, false );
            spanStart = spanEnd + 1;    // ! The loop increment will do "spanEnd = spanStart".

        } else {

            // See if this is a numeric escape sequence for a prohibited ASCII control.

            XMP_Assert ( *spanEnd == '&' );
            int escLen = CountControlEscape ( spanEnd, bufEnd );

            if ( escLen < 0 ) {

                // Have a partial numeric escape in this buffer, wait for more input.
                if ( last ) continue;   // No more buffers, not an escape, absorb as normal input.
                xmlParser->ParseBuffer ( spanStart, (spanEnd - spanStart), false );
                return (spanEnd - buffer);

            } else if ( escLen > 0 ) {

                // Have a complete numeric escape to replace.
                xmlParser->ParseBuffer ( spanStart, (spanEnd - spanStart), false );
                xmlParser->ParseBuffer ( " ", 1, false );
                spanStart = spanEnd + escLen;
                spanEnd = spanStart - 1;    // ! The loop continuation will increment spanEnd!

            }

        }

    }

    XMP_Assert ( spanEnd == bufEnd );

    if ( spanStart < bufEnd ) xmlParser->ParseBuffer ( spanStart, (spanEnd - spanStart), false );
    if ( last ) xmlParser->ParseBuffer ( " ", 1, true );

    return length;

}   // ProcessUTF8Portion


// -------------------------------------------------------------------------------------------------
// ProcessXMLBuffer
// ----------------
//
// Process one buffer of XML. Returns false if this input is put into the pending input buffer.

bool XMPMeta::ProcessXMLBuffer ( XMP_StringPtr buffer, XMP_StringLen xmpSize, bool lastClientCall )
{

    // Determine the character encoding before doing any real parsing. This is needed to do the
    // 8-bit special processing. This has to be checked on every call, not just the first, in
    // order to handle the edge case of single byte buffers.

    XMLParserAdapter & parser = *this->xmlParser;

    if ( parser.charEncoding == XMP_OptionBits(-1) ) {

        if ( (parser.pendingCount == 0) && (xmpSize >= kXMLPendingInputMax) ) {

            // This ought to be the common case, the first buffer is big enough.
            parser.charEncoding = DetermineInputEncoding ( (XMP_Uns8*)buffer, xmpSize );

        } else {

            // Try to fill the pendingInput buffer before calling DetermineInputEncoding.

            size_t pendingOverlap = kXMLPendingInputMax - parser.pendingCount;
            if ( pendingOverlap > xmpSize ) pendingOverlap = xmpSize;

            memcpy ( &parser.pendingInput[parser.pendingCount], buffer, pendingOverlap );   // AUDIT: Count is safe.
            buffer += pendingOverlap;
            xmpSize -= pendingOverlap;
            parser.pendingCount += pendingOverlap;

            if ( (! lastClientCall) && (parser.pendingCount < kXMLPendingInputMax) ) return false;  // Wait for the next buffer.
            parser.charEncoding = DetermineInputEncoding ( parser.pendingInput, parser.pendingCount );

            #if Trace_ParsingHackery
                fprintf ( stderr, "XMP Character encoding is %d\n", parser.charEncoding );
            #endif

        }

    }

    // We have the character encoding. Process UTF-16 and UTF-32 as is. UTF-8 needs special
    // handling to take care of things like ISO Latin-1 or unescaped ASCII controls.

    XMP_Assert ( parser.charEncoding != XMP_OptionBits(-1) );

    if ( parser.charEncoding != kXMP_EncodeUTF8 ) {

        if ( parser.pendingCount > 0 ) {
            // Might have pendingInput from the above portion to determine the character encoding.
            parser.ParseBuffer ( parser.pendingInput, parser.pendingCount, false );
        }
        parser.ParseBuffer ( buffer, xmpSize, lastClientCall );

    } else {

        #if Trace_ParsingHackery
            fprintf ( stderr, "Parsing %d bytes @ %.8X, %s, %d pending, context: %.8s\n",
                      xmpSize, buffer, (lastClientCall ? "last" : "not last"), parser.pendingCount, buffer );
        #endif

        // The UTF-8 processing is a bit complex due to the need to tolerate ISO Latin-1 input.
        // This is done by scanning the input for byte sequences that are not valid UTF-8,
        // assuming they are Latin-1 characters in the range 0x80..0xFF. This requires saving a
        // pending input buffer to handle partial UTF-8 sequences at the end of a buffer.

        while ( parser.pendingCount > 0 ) {

            // We've got some leftover input, process it first then continue with the current
            // buffer. Try to fill the pendingInput buffer before parsing further. We use a loop
            // for wierd edge cases like a 2 byte input buffer, using 1 byte for pendingInput,
            // then having a partial UTF-8 end and need to absorb more.

            size_t pendingOverlap = kXMLPendingInputMax - parser.pendingCount;
            if ( pendingOverlap > xmpSize ) pendingOverlap = xmpSize;

            memcpy ( &parser.pendingInput[parser.pendingCount], buffer, pendingOverlap );   // AUDIT: Count is safe.
            parser.pendingCount += pendingOverlap;
            buffer += pendingOverlap;
            xmpSize -= pendingOverlap;

            if ( (! lastClientCall) && (parser.pendingCount < kXMLPendingInputMax) ) return false;  // Wait for the next buffer.
            size_t bytesDone = ProcessUTF8Portion ( xmlParser, parser.pendingInput, parser.pendingCount, lastClientCall );
            size_t bytesLeft = parser.pendingCount - bytesDone;

            #if Trace_ParsingHackery
                fprintf ( stderr, "   ProcessUTF8Portion handled %d pending bytes\n", bytesDone );
            #endif

            if ( bytesDone == parser.pendingCount ) {

                // Done with all of the pending input, move on to the current buffer.
                parser.pendingCount = 0;

            } else if ( bytesLeft <= pendingOverlap ) {

                // The leftover pending input all came from the current buffer. Exit this loop.
                buffer -= bytesLeft;
                xmpSize += bytesLeft;
                parser.pendingCount = 0;

            } else if ( xmpSize > 0 ) {

                // Pull more of the current buffer into the pending input and try again.
                // Backup by this pass's overlap so the loop entry code runs OK.
                parser.pendingCount -= pendingOverlap;
                buffer -= pendingOverlap;
                xmpSize += pendingOverlap;

            } else {

                // There is no more of the current buffer. Wait for more. Partial sequences at
                // the end of the last buffer should be treated as Latin-1 by ProcessUTF8Portion.
                XMP_Assert ( ! lastClientCall );
                parser.pendingCount = bytesLeft;
                memcpy ( &parser.pendingInput[0], &parser.pendingInput[bytesDone], bytesLeft ); // AUDIT: Count is safe.
                return false;   // Wait for the next buffer.

            }

        }

        // Done with the pending input, process the current buffer.

        size_t bytesDone = ProcessUTF8Portion ( xmlParser, (XMP_Uns8*)buffer, xmpSize, lastClientCall );

        #if Trace_ParsingHackery
            fprintf ( stderr, "   ProcessUTF8Portion handled %d additional bytes\n", bytesDone );
        #endif

        if ( bytesDone < xmpSize ) {

            XMP_Assert ( ! lastClientCall );
            size_t bytesLeft = xmpSize - bytesDone;
            if ( bytesLeft > kXMLPendingInputMax ) XMP_Throw ( "Parser bytesLeft too large", kXMPErr_InternalFailure );

            memcpy ( parser.pendingInput, &buffer[bytesDone], bytesLeft );  // AUDIT: Count is safe.
            parser.pendingCount = bytesLeft;
            return false;   // Wait for the next buffer.

        }

    }

    return true;    // This buffer has been processed.

}   // ProcessXMLBuffer


// -------------------------------------------------------------------------------------------------
// ProcessXMLTree
// --------------

void XMPMeta::ProcessXMLTree ( XMP_OptionBits options )
{

    #if XMP_DebugBuild && DumpXMLParseTree
        if ( this->xmlParser->parseLog == 0 ) this->xmlParser->parseLog = stdout;
        DumpXMLTree ( this->xmlParser->parseLog, this->xmlParser->tree, 0 );
    #endif

    const XML_Node * xmlRoot = FindRootNode ( *this->xmlParser, options );

    if ( xmlRoot != 0 ) {

        this->ProcessRDF ( *xmlRoot, options );

        NormalizeDCArrays ( &this->tree );
        if ( this->tree.options & kXMP_PropHasAliases ) MoveExplicitAliases ( &this->tree, options, this->errorCallback );
        TouchUpDataModel ( this, this->errorCallback );

        // Delete empty schema nodes. Do this last, other cleanup can make empty schema.
        size_t schemaNum = 0;
        while ( schemaNum < this->tree.children.size() ) {
            XMP_Node * currSchema = this->tree.children[schemaNum];
            if ( currSchema->children.size() > 0 ) {
                ++schemaNum;
            } else {
                delete this->tree.children[schemaNum];  // ! Delete the schema node itself.
                this->tree.children.erase ( this->tree.children.begin() + schemaNum );
            }
        }

    }

}   // ProcessXMLTree


// -------------------------------------------------------------------------------------------------
// ParseFromBuffer
// ---------------
//
// Although most clients will probably parse everything in one call, we have a buffered API model
// and need to support even the extreme case of 1 byte at a time parsing. This is considerably
// complicated by some special cases for 8-bit input. Because of this, the first thing we do is
// determine whether the input is 8-bit, UTF-16, or UTF-32.
//
// Both the 8-bit special cases and the encoding determination are easier to do with 8 bytes or more
// of input. The XMLParserAdapter class has a pending-input buffer for this. At the start of parsing
// we (might) try to fill this buffer before determining the input character encoding. After that,
// we (might) use this buffer with the current input to simplify the logic in Process8BitInput. The
// "(might)" part means that we don't actually use the pending-input buffer unless we have to. In
// particular, the common case of single-buffer parsing won't use it.

void
XMPMeta::ParseFromBuffer ( XMP_StringPtr  buffer,
                           XMP_StringLen  xmpSize,
                           XMP_OptionBits options )
{
    if ( (buffer == 0) && (xmpSize != 0) ) XMP_Throw ( "Null parse buffer", kXMPErr_BadParam );
    if (xmpSize == kXMP_UseNullTermination) xmpSize = static_cast<XMP_Index>(strnlen_safe(buffer, Max_XMP_Uns32));

    const bool lastClientCall = ((options & kXMP_ParseMoreBuffers) == 0);   // *** Could use FlagIsSet & FlagIsClear macros.

    if ( this->xmlParser == 0 ) {
        this->tree.ClearNode(); // Make sure the target XMP object is totally empty.
        if ( (xmpSize == 0) && lastClientCall ) return; // Tolerate empty parse. Expat complains if there are no XML elements.
        this->xmlParser = XMP_NewExpatAdapter ( ExpatAdapter::kUseGlobalNamespaces );
        this->xmlParser->SetErrorCallback ( &this->errorCallback );
    }

    try {   // Cleanup the tree and xmlParser if anything fails.

        bool done = this->ProcessXMLBuffer ( buffer, xmpSize, lastClientCall );
        if ( ! done ) return;   // Wait for the next buffer.

        if ( lastClientCall ) {
            this->ProcessXMLTree ( options );
            delete this->xmlParser;
            this->xmlParser = 0;
        }

    } catch ( ... ) {

        delete this->xmlParser;
        this->xmlParser = 0;
        throw;

    }

}   // ParseFromBuffer

// =================================================================================================
