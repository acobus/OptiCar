/**
 *
 * ADTF Template Project Filter.
 *
 * @file
 * Copyright &copy; Audi Electronics Venture GmbH. All rights reserved
 *
 * $Author: belkera $
 * $Date: 2011-06-30 16:51:21 +0200 (Do, 30 Jun 2011) $
 * $Revision: 26514 $
 *
 * @remarks
 *
 */
#ifndef _TEMPLATE_PROJECT_FILTER_H_
#define _TEMPLATE_PROJECT_FILTER_H_

#define OID_ADTF_TEMPLATE_FILTER "adtf.example.template_filter"


//*************************************************************************************************
class cTemplateProjectFilter : public adtf::cFilter
{
    ADTF_FILTER(OID_ADTF_TEMPLATE_FILTER, "Template Project Filter", adtf::OBJCAT_DataFilter);

protected:
    cInputPin    m_oTemplateInput;
    cOutputPin    m_oTemplateOutput;

public:
    cTemplateProjectFilter(const tChar* __info);
    virtual ~cTemplateProjectFilter();

protected:
    tResult Init(tInitStage eStage, __exception);
    tResult Shutdown(tInitStage eStage, __exception);

    // implements IPinEventSink
    tResult OnPinEvent(IPin* pSource,
                       tInt nEventCode,
                       tInt nParam1,
                       tInt nParam2,
                       IMediaSample* pMediaSample);
};

//*************************************************************************************************
#endif // _TEMPLATE_PROJECT_FILTER_H_
