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

#ifndef OPENALPR_PLUGIN_H
#define OPENALPR_PLUGIN_H

#include <stdexcept>
#include <fstream>
#include <algorithm>

#include <alpr.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <boost/program_options.hpp>
#include <boost/program_options/option.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/errors.hpp>

#include <zoneminder/zm_plugin_manager.h>
#include <zoneminder/zm_detector.h>
#include <zoneminder/zm_rgb.h>

#define DEFAULT_CONFIG_FILE "/etc/openalpr/openalpr.conf"
#define DEFAULT_COUNTRY_CODE "us"
#define DEFAULT_TEMPLATE_REGION ""
#define DEFAULT_TOPN 10
#define DEFAULT_DETECT_REGION 0
#define DEFAULT_DET_CAUSE "Plate Detected"
#define DEFAULT_PLUGIN_LOG_PREFIX "OPENALPR PLUGIN"

#define DEFAULT_ALARM_SCORE 99
#define DEFAULT_MIN_CONFIDENCE 0
#define DEFAULT_ADAPTIVE_CONF 0
#define DEFAULT_MIN_CHARACTERS 0
#define DEFAULT_MAX_CHARACTERS 99
#define DEFAULT_MAX_EXCL_PERIOD 0

using namespace std;
using namespace alpr;
using namespace boost::program_options;

//! OpenALPR plugin class.
/*! The class derived from Detector.
 *  This class provides license plate detection based on tesseract OCR.
 */
class OpenALPRPlugin : public Detector {
  public:

    //! Default Constructor.
    OpenALPRPlugin();

    //! Constructor.
    OpenALPRPlugin(string sConfigSectionName);

    //! Destructor.
    virtual ~OpenALPRPlugin();

    //! Copy constructor.
    OpenALPRPlugin(const OpenALPRPlugin& source);

    //! Overloaded operator=.
    OpenALPRPlugin& operator=(const OpenALPRPlugin& source);

    int loadConfig(string sConfigFileName, map<unsigned int,map<string,string> > mapPluginConf);

protected:

    void onCreateEvent(Zone *zone, unsigned int n_zone, Event *event);
    void onCloseEvent(Zone *zone, unsigned int n_zone, Event *event);
    bool checkZone(Zone *zone, unsigned int n_zone, Event *event);

    bool plateIsExcluded(string plateName);
    void addPlate(string plateNum, float confidence);

    string m_sConfigFilePath;
    string m_sCountry;
    string m_sRegionTemplate;
    unsigned int m_nMaxPlateNumber;
    bool m_bRegionIsDet;

private:

    Alpr *ptrAlpr;

    struct pConf
    {
        unsigned int minConfidence;
        bool adaptiveConf;
        unsigned int minCharacters;
        unsigned int maxCharacters;
        unsigned int maxExclPeriod;
        unsigned int alarmScore;
        pConf():
            minConfidence(DEFAULT_MIN_CONFIDENCE),
            adaptiveConf(DEFAULT_ADAPTIVE_CONF),
            minCharacters(DEFAULT_MIN_CHARACTERS),
            maxCharacters(DEFAULT_MAX_CHARACTERS),
            maxExclPeriod(DEFAULT_MAX_EXCL_PERIOD),
            alarmScore(DEFAULT_ALARM_SCORE)
        {}
    };

    vector<pConf> pluginConfig;

    struct tsPlate
    {
        time_t ts;
        string num;
    };

    struct strPlate
    {
        string num;
        float conf;
    };

    vector<tsPlate> recentPlates;
    vector<strPlate> plateList;

    float topConfidence;

    class findOlderThan
    {
        time_t _ts;
    public:
        findOlderThan(const time_t &ts) : _ts(ts) {}
        bool operator()(const tsPlate &item) const
        {
            return item.ts < _ts;
        }
    };

};
#endif // OPENALPR_PLUGIN_H
