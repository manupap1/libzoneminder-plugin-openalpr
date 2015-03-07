/*****************************************************************************
 * Copyright (C) 2014 Emmanuel Papin <manupap01@gmail.com>
 *
 * Authors: Emmanuel Papin <manupap01@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#include "openalpr_plugin.h"

//! Retrieve the engine version we're going to expect
extern "C" int getEngineVersion()
{
  return ZM_ENGINE_VERSION;
}

//! Tells us to register our functionality to an engine kernel
extern "C" void registerPlugin(PluginManager &PlM, string sPluginName)
{
    PlM.getImageAnalyser().addDetector(auto_ptr<Detector>(new OpenALPRPlugin(sPluginName)));
}

OpenALPRPlugin::OpenALPRPlugin()
  : Detector(),
    m_sConfigFilePath(DEFAULT_CONFIG_FILE),
    m_sCountry(DEFAULT_COUNTRY_CODE),
    m_sRegionTemplate(DEFAULT_TEMPLATE_REGION),
    m_nMaxPlateNumber(DEFAULT_TOPN),
    m_bRegionIsDet(DEFAULT_DETECT_REGION)
{
    m_sDetectionCause = DEFAULT_DETECTION_CAUSE;
    m_sLogPrefix = DEFAULT_PLUGIN_LOG_PREFIX;

    Info("%s: Plugin object has been created", m_sLogPrefix.c_str());
}

OpenALPRPlugin::OpenALPRPlugin(string sPluginName)
  : Detector(sPluginName),
    m_sConfigFilePath(DEFAULT_CONFIG_FILE),
    m_sCountry(DEFAULT_COUNTRY_CODE),
    m_sRegionTemplate(DEFAULT_TEMPLATE_REGION),
    m_nMaxPlateNumber(DEFAULT_TOPN),
    m_bRegionIsDet(DEFAULT_DETECT_REGION)
{
    m_sDetectionCause = DEFAULT_DETECTION_CAUSE;
    m_sLogPrefix = DEFAULT_PLUGIN_LOG_PREFIX;

    Info("%s: Plugin object has been created", m_sLogPrefix.c_str());
}

/*! \fn OpenALPRPlugin::loadConfig(string sConfigFileName, map<unsigned int,map<string,string> > mapPluginConf)
 *  \param sConfigFileName is path to configuration to load parameters from
 *  \param mapPluginConf is the map of configuration parameters for each zone
*/
int OpenALPRPlugin::loadConfig(string sConfigFileName, map<unsigned int,map<string,string> > mapPluginConf)
{
    try
    {
        options_description config_file("Configuration file options.");
        variables_map vm;
        config_file.add_options()
            ((m_sConfigSectionName + string(".config_file")).c_str(),
                value<string>()->default_value(DEFAULT_CONFIG_FILE))
            ((m_sConfigSectionName + string(".country_code")).c_str(),
                value<string>()->default_value(DEFAULT_COUNTRY_CODE))
            ((m_sConfigSectionName + string(".template_region")).c_str(),
                value<string>()->default_value(DEFAULT_TEMPLATE_REGION))
            ((m_sConfigSectionName + string(".topn")).c_str(),
                value<int>()->default_value(DEFAULT_TOPN))
            ((m_sConfigSectionName + string(".detect_region")).c_str(),
                value<bool>()->default_value(DEFAULT_DETECT_REGION))
            ((m_sConfigSectionName + string(".det_cause")).c_str(),
                value<string>()->default_value(DEFAULT_DET_CAUSE))
            ((m_sConfigSectionName + string(".log_prefix")).c_str(),
                value<string>()->default_value(DEFAULT_PLUGIN_LOG_PREFIX));
        try
        {
            ifstream ifs(sConfigFileName.c_str());
            store(parse_config_file(ifs, config_file, false), vm);
            notify(vm);
        }
        catch(error& er)
        {
            Error("%s: Plugin is not configured (%s)", m_sLogPrefix.c_str(), er.what());
            return 0;
        }
        m_sConfigFilePath = vm[(m_sConfigSectionName + string(".config_file")).c_str()].as<string>();
        m_sCountry = vm[(m_sConfigSectionName + string(".country_code")).c_str()].as<string>();
        m_sRegionTemplate = vm[(m_sConfigSectionName + string(".template_region")).c_str()].as<string>();
        m_nMaxPlateNumber = vm[(m_sConfigSectionName + string(".topn")).c_str()].as<int>();
        m_bRegionIsDet = vm[(m_sConfigSectionName + string(".detect_region")).c_str()].as<bool>();
        m_sDetectionCause = vm[(m_sConfigSectionName + string(".det_cause")).c_str()].as<string>();
        m_sLogPrefix = vm[(m_sConfigSectionName + string(".log_prefix")).c_str()].as<string>();
    }
    catch(exception& ex)
    {
        Error("%s: Plugin is not configured (%s)", m_sLogPrefix.c_str(), ex.what());
        return 0;
    }

    pConf pluginConf;
    for (map<unsigned int,map<string,string> >::iterator it = mapPluginConf.begin(); it != mapPluginConf.end(); ++it) {
        while ( pluginConfig.size() < (it->first + 1) )
            pluginConfig.push_back(pluginConf);
        // Overwrite default values with database values
        for (map<string,string>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
            if (it2->second.empty()) continue;
            if (it2->first == "MinConfidence") {
                pluginConfig[it->first].minConfidence = (unsigned int)strtoul(it2->second.c_str(), NULL, 0);
            } else if (it2->first == "MinCharacters") {
                pluginConfig[it->first].minCharacters = (unsigned int)strtoul(it2->second.c_str(), NULL, 0);
            } else if (it2->first == "MaxCharacters") {
                pluginConfig[it->first].maxCharacters = (unsigned int)strtoul(it2->second.c_str(), NULL, 0);
            } else if (it2->first == "ExclPeriod") {
                pluginConfig[it->first].ExclPeriod = (unsigned int)strtoul(it2->second.c_str(), NULL, 0);
            } else if (it2->first == "AlarmScore") {
                pluginConfig[it->first].alarmScore = (unsigned int)strtoul(it2->second.c_str(), NULL, 0);
            }
        }
    }

    // Create an instance of class Alpr and set basic configuration
    ptrAlpr = new Alpr(m_sCountry, m_sConfigFilePath);
    ptrAlpr->setTopN(m_nMaxPlateNumber);
    if ( m_bRegionIsDet )
        ptrAlpr->setDetectRegion(m_bRegionIsDet);
    if ( !m_sRegionTemplate.empty() )
        ptrAlpr->setDefaultRegion(m_sRegionTemplate);

    // Initialize some lists
    int nb_zones = pluginConfig.size();
    plateList.resize(nb_zones);
    tmpPlateList.resize(nb_zones);
    exclPlateList.resize(nb_zones);
    lastEventDates.resize(nb_zones);

    if ( ptrAlpr->isLoaded() ) {
        Info("%s: Plugin is configured", m_sLogPrefix.c_str());
    } else {
        Error("%s: Plugin is not configured (%s)", m_sLogPrefix.c_str(), strerror(errno));
        delete ptrAlpr;
        return 0;
    }

    return 1;
}


OpenALPRPlugin::~OpenALPRPlugin()
{
    delete ptrAlpr;
}


/*! \fn OpenALPRPlugin::OpenALPRPlugin(const OpenALPRPlugin& source)
 *  \param source is the object for copying
 */
OpenALPRPlugin::OpenALPRPlugin(const OpenALPRPlugin& source)
  : Detector(source),
    m_sConfigFilePath(source.m_sConfigFilePath),
    m_sCountry(source.m_sCountry),
    m_sRegionTemplate(source.m_sRegionTemplate),
    m_nMaxPlateNumber(source.m_nMaxPlateNumber),
    m_bRegionIsDet(source.m_bRegionIsDet)
{
}


/*! \fn OpenALPRPlugin:: operator=(const OpenALPRPlugin& source)
 *  \param source is the object for copying
 */
OpenALPRPlugin & OpenALPRPlugin:: operator=(const OpenALPRPlugin& source)
{
    Detector::operator=(source);
    m_sConfigFilePath = source.m_sConfigFilePath;
    m_sCountry = source.m_sCountry;
    m_sRegionTemplate = source.m_sRegionTemplate;
    m_nMaxPlateNumber = source.m_nMaxPlateNumber;
    m_bRegionIsDet = source.m_bRegionIsDet;
    return *this;
}


void OpenALPRPlugin::onCreateEvent(Zone *zone, unsigned int n_zone, Event *event)
{
    Debug(1, "%s: Zone %s - Prepare plugin for event %d", m_sLogPrefix.c_str(), zone->Label(), event->Id());
    plateList[n_zone].clear();
    tmpPlateList[n_zone].clear();

    /* FIXME: ExclPeriod feature is bugged
    if (exclPlateList[n_zone].size() > 0)
    {
        if ((time(0) - pluginConfig[n_zone].ExclPeriod) > lastEventDates[n_zone])
        {
            Debug(1, "%s: Zone %s - Clear list of excluded plates (period expired)", m_sLogPrefix.c_str(), zone->Label());
            exclPlateList[n_zone].clear();
        }
    }*/
}


void OpenALPRPlugin::onCloseEvent(Zone *zone, unsigned int n_zone, Event *event)
{
    Event::StringSetMap noteSetMap;
    Event::StringSet noteSet;

    // Sort plates according confidence level (higher first)
    sort(plateList[n_zone].begin(), plateList[n_zone].end(), sortByConf());

    // Set the number of plates to output
    unsigned int topn = ( m_nMaxPlateNumber < plateList[n_zone].size() ) ? m_nMaxPlateNumber : plateList[n_zone].size();

    // Delete event and exit if no plate to output
    if (topn == 0)
    {
        Debug(1, "%s: Zone %s - Delete event %d (no retained plates)", m_sLogPrefix.c_str(), zone->Label(),  event->Id());
        event->DeleteEvent();
        return;
    }

    // Set date of event
    lastEventDates[n_zone] = time(0);

    /* FIXME: ExclPeriod feature is bugged
    // Keep the list of plate to exclude for next event
    exclPlateList[n_zone] = tmpPlateList[n_zone];
    Debug(1, "%s: Zone %s - %u plate(s) in exclusion list", m_sLogPrefix.c_str(), zone->Label(), exclPlateList[n_zone].size());*/

    // Display zone name
    string sOutput = "[Zone ";
    sOutput += zone->Label();
    sOutput += "]\n";

    // Keep only the topn first plates with higher confidence
    for(unsigned int i=0; i<topn;i++)
    {
        std::stringstream plate;
        plate << plateList[n_zone][i].num << " (" << plateList[n_zone][i].conf << ")";
        Debug(1, "%s: Zone %s - Plate %s detected", m_sLogPrefix.c_str(), zone->Label(), plate.str().c_str());
        sOutput += "   " + plate.str() + "\n";
    }

    // Add plates to event's note
    noteSet.insert(sOutput);
    noteSetMap[m_sDetectionCause] = noteSet;
    Info("%s: Zone %s - Add plates to event %d", m_sLogPrefix.c_str(), zone->Label(), event->Id());
    event->updateNotes(noteSetMap);
}


/*! \fn OpenALPRPlugin::checkZone(Zone *zone, const Image *zmImage)
 *  \param zone is a zone where license plates will be detected
 *  \param zmImage is an image to perform license plate detection (in the form of ZM' Image)
 *  \return true if there were objects detected in given image and false otherwise
 */
bool OpenALPRPlugin::checkZone(Zone *zone, unsigned int n_zone, const Image *zmImage)
{
    Polygon zone_polygon = Polygon(zone->GetPolygon()); // Polygon of interest of the processed zone.

    Image *pMaskImage = new Image(zmImage->Width(), zmImage->Height(), ZM_COLOUR_GRAY8, ZM_SUBPIX_ORDER_NONE );
    pMaskImage->Fill(BLACK);
    // An temporary image in the form of ZM for making from it CvMat.
    // If don't use temp image, after rgb->bgr it will change.
    Image *tempZmImage = new Image(*zmImage);
    int imgtype=CV_8UC1;
    if (tempZmImage->Colours() == ZM_COLOUR_RGB24)
      imgtype=CV_8UC3;
    cv::Mat cvInputImage = cv::Mat(
                           tempZmImage->Height(),
                           tempZmImage->Width(),
                           imgtype, (unsigned char*)tempZmImage->Buffer()).clone();
    if (tempZmImage->Colours() == ZM_COLOUR_RGB24)
        cvtColor(cvInputImage, cvInputImage, CV_RGB2BGR);

    //Process image
    //std::vector<unsigned char> buffer;
    //cv::imencode(".bmp", cvInputImage, buffer);
    std::vector<AlprRegionOfInterest> regionsOfInterest;
    regionsOfInterest.push_back(AlprRegionOfInterest(0,0, cvInputImage.cols, cvInputImage.rows));
    // Region of interest don't work as expected
    //std::vector<AlprResults> results = ptrAlpr->recognize(buffer, regionsOfInterest);
    AlprResults results = ptrAlpr->recognize(cvInputImage.data, cvInputImage.elemSize(), cvInputImage.cols, cvInputImage.rows, regionsOfInterest);
    double score = 0;

    for (unsigned int i = 0; i < results.plates.size(); i++) {
        int x1 = results.plates[i].plate_points[0].x, y1 = results.plates[i].plate_points[0].y;
        int x2 = results.plates[i].plate_points[1].x, y2 = results.plates[i].plate_points[1].y;
        int x3 = results.plates[i].plate_points[2].x, y3 = results.plates[i].plate_points[2].y;
        int x4 = results.plates[i].plate_points[3].x, y4 = results.plates[i].plate_points[3].y;
        Coord rectVertCoords[4] = {Coord(x1, y1), Coord(x2, y2), Coord(x3, y3), Coord(x4, y4)};
        int nNumVertInside = 0;

        for (int p = 0; p < 4; p++)
            nNumVertInside += zone_polygon.isInside(rectVertCoords[p]);

        // if at least three rectangle coordinates are inside polygon,
        // consider rectangle as belonging to the zone
        // otherwise process next object
        if (nNumVertInside < 3)
        {
            Debug(1, "%s: Zone %s - Skip result (outside detection zone)", m_sLogPrefix.c_str(), zone->Label());
            continue;
        }

        int cntDetPlates = 0;

        for (unsigned int k = 0; k < results.plates[i].topNPlates.size(); k++)
        {
            strPlate detPlate;
            detPlate.num = results.plates[i].topNPlates[k].characters;
            detPlate.conf = results.plates[i].topNPlates[k].overall_confidence;

            // Disqualify plate if under the minimum confidence level
            if (detPlate.conf < pluginConfig[n_zone].minConfidence)
            {
                Debug(1, "%s: Zone %s - Skip plate %s (under minimum confidence level)", m_sLogPrefix.c_str(), zone->Label(), detPlate.num.c_str());
                continue;
            }

            // Disqualify plate if not enough characters or too much characters
            if ((detPlate.num.size() < pluginConfig[n_zone].minCharacters)
                    || (detPlate.num.size() > pluginConfig[n_zone].maxCharacters))
            {
                Debug(1, "%s: Zone %s - Skip plate %s (number of characters is out of range)", m_sLogPrefix.c_str(), zone->Label(), detPlate.num.c_str());
                continue;
            }

            /* FIXME: ExclPeriod feature is bugged
            // Disqualify plate if in exclusion period
            if (plateIsExcluded(n_zone, detPlate.num))
            {
                Debug(1, "%s: Zone %s - Skip plate %s (exists in exclusion list)", m_sLogPrefix.c_str(), zone->Label(), detPlate.num.c_str());
                continue;
            }*/

            // Add plate to list (if already in list, update confidence by adding new value)
            addPlate(zone, n_zone, detPlate);

            cntDetPlates++;
        }

        if (cntDetPlates == 0)
            continue;

        // Raise an alarm if at least one plate has been detected
        score = pluginConfig[n_zone].alarmScore;

        // Fill a polygon with object in the mask
        Polygon platePolygon = Polygon(4, rectVertCoords);
        pMaskImage->Fill(WHITE, 1, platePolygon);
    }

    if (score == 0) {
        delete pMaskImage;
        delete tempZmImage;
        return false;
    }

    zone->SetScore((int)score);

    //Get mask by highlighting contours of objects and overlaying them with previous contours.
    Rgb alarm_colour = RGB_GREEN;
    Image *hlZmImage = pMaskImage->HighlightEdges(alarm_colour, ZM_COLOUR_RGB24,
                                                 ZM_SUBPIX_ORDER_RGB, &zone_polygon.Extent());

    if (zone->Alarmed()) {
        // if there were previous detection and they have already set up alarm image
        // then overlay it with current mask
        Image* pPrevZoneMask = new Image(*(zone->AlarmImage()));
        pPrevZoneMask->Overlay(*hlZmImage);
        zone->SetAlarmImage(pPrevZoneMask);
        delete pPrevZoneMask;
    } else
        zone->SetAlarmImage(hlZmImage);

    delete pMaskImage;
    delete tempZmImage;

    return true;
}

//FIXME: ExclPeriod feature is bugged
bool OpenALPRPlugin::plateIsExcluded(unsigned int n_zone, string plateNum)
{
    for(vector<string>::iterator it = exclPlateList[n_zone].begin(); it != exclPlateList[n_zone].end(); ++it)
        if (*it == plateNum)
            return true;

    return false;
}

bool OpenALPRPlugin::addPlate(Zone *zone, unsigned int n_zone, strPlate detPlate)
{
    for (vector<strPlate>::iterator it = plateList[n_zone].begin(); it != plateList[n_zone].end(); ++it)
    {
        // If plate number exists in the list for this zone
        if ((*it).num == detPlate.num)
        {
            // Add confidence
            (*it).conf += detPlate.conf;
            Debug(1, "%s: Zone %s - Raise confidence of plate %s to %f", m_sLogPrefix.c_str(), zone->Label(), (*it).num.c_str(), (*it).conf);
            return false;
        }
    }

    // Add a new plate for this zone
    Debug(1, "%s: Zone %s - Add plate %s with confidence %f", m_sLogPrefix.c_str(), zone->Label(), detPlate.num.c_str(), detPlate.conf);
    plateList[n_zone].push_back(detPlate);

    /* FIXME: ExclPeriod feature is bugged (with the reference video monitored in loop, plates are no more detected after some hours)
    // Add plate to exclusion list if an exclusion period is set
    if (pluginConfig[n_zone].ExclPeriod > 0)
    {
        Debug(1, "%s: Zone %s - Add plate %s to exclusion list", m_sLogPrefix.c_str(), zone->Label(), detPlate.num.c_str());
        tmpPlateList[n_zone].push_back(detPlate.num);
    }*/

    return true;
}
