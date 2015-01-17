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

    Info("OpenALPR plugin object has been created.");
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

    Info("OpenALPR plugin object has been created.");
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
            Error("Plugin is not configured (%s)", er.what());
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
        Error("Plugin is not configured (%s)", ex.what());
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
            } else if (it2->first == "AdaptiveConf") {
                pluginConfig[it->first].adaptiveConf = (it2->second == "yes");
            } else if (it2->first == "MinCharacters") {
                pluginConfig[it->first].minCharacters = (unsigned int)strtoul(it2->second.c_str(), NULL, 0);
            } else if (it2->first == "MaxCharacters") {
                pluginConfig[it->first].maxCharacters = (unsigned int)strtoul(it2->second.c_str(), NULL, 0);
            } else if (it2->first == "MaxExclPeriod") {
                pluginConfig[it->first].maxExclPeriod = (unsigned int)strtoul(it2->second.c_str(), NULL, 0);
            } else if (it2->first == "AlarmScore") {
                pluginConfig[it->first].alarmScore = (unsigned int)strtoul(it2->second.c_str(), NULL, 0);
            }
        }
    }

    ptrAlpr = new Alpr(m_sCountry, m_sConfigFilePath);
    ptrAlpr->setTopN(m_nMaxPlateNumber);
    if ( m_bRegionIsDet )
        ptrAlpr->setDetectRegion(m_bRegionIsDet);
    if ( !m_sRegionTemplate.empty() )
        ptrAlpr->setDefaultRegion(m_sRegionTemplate);

    topConfidence = 0;

    if ( ptrAlpr->isLoaded() ) {
        Info("Plugin is configured.");
    } else {
        Error("Plugin is not configured (%s)", strerror(errno));
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
    Info("OpenALPRPlugin::onCreateEvent");
}

void OpenALPRPlugin::onCloseEvent(Zone *zone, unsigned int n_zone, Event *event)
{
    Info("OpenALPRPlugin::onCloseEvent");
}

/*! \fn OpenALPRPlugin::checkZone(Zone *zone, const Image *zmImage)
 *  \param zone is a zone where license plates will be detected
 *  \param zmImage is an image to perform license plate detection (in the form of ZM' Image)
 *  \return true if there were objects detected in given image and false otherwise
 */
bool OpenALPRPlugin::checkZone(Zone *zone, unsigned int n_zone, const Image *zmImage)
{
    double score;
    string sOutput;
    Polygon zone_polygon = Polygon(zone->GetPolygon()); // Polygon of interest of the processed zone.

    // Get region of interest for the processed zone
    // Currently openalpr only support rectangle region of interests
    //std::vector<AlprRegionOfInterest> regionsOfInterest;
    //AlprRegionOfInterest region = {zone_polygon.Extent().LoX(), zone_polygon.Extent().LoY(), zone_polygon.Extent().Width(), zone_polygon.Extent().Height()};
    //regionsOfInterest.push_back(region);
    //Info("RegionOfInterest: (LoX=%d, LoY=%d) - (Width=%d, Height=%d)", zone_polygon.Extent().LoX(), zone_polygon.Extent().LoY(), zone_polygon.Extent().Width(), zone_polygon.Extent().Height());

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
    score = 0;

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
            continue;

        int cntDetInArea = 0;
        time_t now = time(0);

        // Remove plates from exclusion list after period expiration
        recentPlates.erase(remove_if(recentPlates.begin(), recentPlates.end(),
                    findOlderThan(now - pluginConfig[n_zone].maxExclPeriod)), recentPlates.end());

        for (unsigned int k = 0; k < results.plates[i].topNPlates.size(); k++) {

            // Disqualify plate if under the minimum confidence level
            if (results.plates[i].topNPlates[k].overall_confidence < pluginConfig[n_zone].minConfidence)
                continue;

            // Disqualify plate if not enough characters or too much characters
            if ((results.plates[i].topNPlates[k].characters.size() < pluginConfig[n_zone].minCharacters)
                    || (results.plates[i].topNPlates[k].characters.size() > pluginConfig[n_zone].maxCharacters))
                continue;

            // Disqualify plate if already in exclusion period
/*            if (plateIsExcluded(results.plates[i].topNPlates[k].characters))
            continue;
*/
            // Set confidence level
/*            if (pluginConfig[n_zone].adaptiveConf) {
                if (topConfidence < results.plates[i].topNPlates[k].overall_confidence)
                {
                    topConfidence = results.plates[i].topNPlates[k].overall_confidence;
                } else
                    continue;
            }

            if (cntDetInArea == 0) {
                std::stringstream title;
                title << "Plate detected #" << i << " with potential matches:";
                sOutput = title.str();
                Info(sOutput.c_str());
                sOutput += "\n";
            }

            std::stringstream result;
            result << "- " << results.plates[i].topNPlates[k].characters
                   << " (confidence: " << results.plates[i].topNPlates[k].overall_confidence
                   << ", template_match: " << results.plates[i].topNPlates[k].matches_template << ")";
            Info("%s", result.str().c_str());
            sOutput += result.str() + "\n";

            // Add plate to exclusion list
            tsPlate newPlate;
            newPlate.ts = now;
            newPlate.num = results.plates[i].topNPlates[k].characters;
            recentPlates.push_back(newPlate);
*/
            addPlate(results.plates[i].topNPlates[k].characters, results.plates[i].topNPlates[k].overall_confidence);
            cntDetInArea++;
        }

        if (cntDetInArea == 0)
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
        return( false );
    }

    zone->SetScore((int)score);
    zone->SetText(sOutput);

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

bool OpenALPRPlugin::plateIsExcluded(string plateName)
{
    for(vector<tsPlate>::iterator it = recentPlates.begin(); it != recentPlates.end(); ++it)
        if ((*it).num == plateName)
            return true;
    return false;
}

void OpenALPRPlugin::addPlate(string plateNum, float confidence)
{
    Info("OpenALPRPlugin::addPlate");
    for(vector<strPlate>::iterator it = plateList.begin(); it != plateList.end(); ++it)
    {
        if ((*it).num == plateNum)
        {
            (*it).conf += confidence;
            return;
        }
    }
    strPlate newPlate;
    newPlate.num = plateNum;
    newPlate.conf = confidence;
    plateList.push_back(newPlate);
}
