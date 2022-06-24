#!/usr/bin/env python3
# -*- coding: iso-8859-1 -*-

# script for checking of the correctness of the catalog files (cd, ct)
# before running the script the submodules must be updated

import re, os, glob, sys, pytz
from datetime import datetime

import chardet

################################################################################

class Module(object):
    def __init__(self, name, required_version):
        self.name = name
        self.required_version = required_version
        self.language_dict = {} # dictionary with language : available version

    def get_version(self, language):
        return self.language_dict[language]

    def get_name(self):
        return self.name
    
    def get_name_as_field(self, len):
        return self.get_name().ljust(len)

    def get_required_version(self):
        return self.required_version
    
    def get_required_version_as_field(self, len):
        version = self.get_required_version()
        if version >= 0:
            return str(version).ljust(len)
        else:
            return ("**n/a**").ljust(len)

    def get_version(self, language):
        if language in self.language_dict:
            return self.language_dict[language]
        else:
            return -1

    def get_version_as_field(self, language, len):
        version = self.get_version(language)
        required_version = self.get_required_version()
        if version >= 0:
            if version < required_version:
                return ("**" + str(version) + "**").ljust(len)
            else:
                return str(version).ljust(len)
        else:
            return ("**n/a**").ljust(len)

    def add_version(self, language, version):
        if language.endswith(".ct"):
            language = language[0:-3]
        # 1st letter upper case
        language = language[0].upper() + language[1:]
        self.language_dict[language] = version

    def get_languages(self, languages):
        for language in self.language_dict:
            if not language in languages:
                languages.append(language)
                
        
################################################################################
        
class Report(object):
    def __init__(self):
        self.modules = [] # list of Module objects

    def add_module(self, module):
        self.modules.append(module)
    
    def compare_version(self, required_version, available_version):
        if required_version >= 0 \
                and available_version >= 0 \
                and required_version == available_version:
            return 1
        return 0

    def write_subtable_rst(self, languages, start, end):
        # create reST table separator
        tablesep = "=" * 59 + " " + "================ "
        for language in range(start, end):
            tablesep += "=" * 14 + " "
        tablesep += "\n"
        
        # print table header
        fh.write(tablesep)
        fh.write("Module" + " " * 54 + "Required Version ")

        for language_index in range(start, end):
            fh.write(f"{languages[language_index]:15}")
        fh.write("\n")
        fh.write(tablesep)

        for module in self.modules:
            fh.write(module.get_name_as_field(60))
            fh.write(module.get_required_version_as_field(17))
            for language_index in range(start, end):
                fh.write(module.get_version_as_field(languages[language_index], 15))
            fh.write("\n")
        
        fh.write(tablesep)
        fh.write("\n\n")
        
    def write_rst(self, fh):
        # create a list with all used languages
        languages = []
        
        for module in self.modules:
            module.get_languages(languages)

        languages.sort()
        
        # we create 3 tables because of its width
        self.write_subtable_rst(languages, 0, 6)
        self.write_subtable_rst(languages, 6, 12)
        self.write_subtable_rst(languages, 12, len(languages))

        tablesep = "========== ========\n"
        fh.write(tablesep)
        fh.write("Language   Done [%]\n")
        fh.write(tablesep)
        # summary per language
        for lang in languages:
            done = 0
            fh.write(lang.ljust(10))
            for module in self.modules:
                required_version = module.get_required_version()
                lang_version = module.get_version(lang)
                done += self.compare_version(required_version, lang_version)
            fh.write(" %.1f" % (done / len(self.modules) * 100.0))
            fh.write("\n")
        fh.write(tablesep)
            

################################################################################

# regex for parsing .gitmodules
re_path = re.compile("^\s*?path = (.*)$", re.MULTILINE)

# regex for parsing ## version in a CT file
re_ct_ver = re.compile(r"""
^\#\#\s+version\s+\$VER:\s+[\w\.]+?\.catalog\s+
(\d+)\.\d+\s+\((\d\d)\.(\d\d)\.(\d\d\d\d)\).*?$
""", re.MULTILINE | re.VERBOSE)

# regex for parsing ## language in a CT file
re_ct_lang = re.compile(r"^##\s+language\s+(\w+)$", re.MULTILINE)

# regex for parsing ## codeset in a CT file
re_ct_code = re.compile(r"^##\s+codeset\s+(\d+)$", re.MULTILINE)

# regex for parsing #defin CATALOG_VERSION in catalog_version.h
re_ct_reqver = re.compile(r"^#define\s+CATALOG_VERSION\s+(\d+)$", re.MULTILINE)


languages = {
    "albanian.ct":      (0, "ISO-8859-1", "unknown"),
    "catalan.ct":       (0, "ISO-8859-1", "català"),
    "danish.ct":        (0, "ISO-8859-1", "dansk"),
    "dutch.ct":         (0, "ISO-8859-1", "nederlands"),
    "faroese.ct":       (0, "ISO-8859-1", "unknown"),
    "finnish.ct":       (0, "ISO-8859-1", "suomi"),
    "french.ct":        (0, "ISO-8859-1", "français"),
    "german.ct":        (0, "ISO-8859-1", "deutsch"),
    "irish.ct":         (0, "ISO-8859-1", "unknown"),
    "icelandic.ct":     (0, "ISO-8859-1", "unknown"),
    "italian.ct":       (0, "ISO-8859-1", "italiano"),
    "norwegian.ct":     (0, "ISO-8859-1", "norsk"),
    "portuguese.ct":    (0, "ISO-8859-1", "português"),
    "spanish.ct":       (0, "ISO-8859-1", "español"),
    "swedish.ct":       (0, "ISO-8859-1", "svenska"),
    "bosnian.ct":       (5, "ISO-8859-2", "unknown"),
    "croatian.ct":      (5, "Windows-1252", "hrvatski"), # Windows-1252 or ISO-8859-2 ?
    "czech.ct":         (5, "ISO-8859-2", "czech"),
    "hungarian.ct":     (5, "ISO-8859-2", "magyar"),
    "rumanian.ct":      (5, "ISO-8859-2", "unknown"),
    "slovak.ct":        (5, "ISO-8859-2", "unknown"),
    "slovene.ct":       (5, "ISO-8859-2", "unknown"),
    "polish.ct":        (5, "ISO-8859-2", "polski"), # AmigaPL or ISO-8859-2 ?
    "maltese.ct":       (6, "ISO-8859-3", "unknown"),
    "estonian.ct":      (7, "ISO-8859-4", "unknown"),
    "latvian.ct":       (7, "ISO-8859-4", "unknown"),
    "lithuanian.ct":    (7, "ISO-8859-4", "unknown"),
    "bulgarian.ct":     (8, "ISO-8859-5", "unknown"),
    "macedonian.ct":    (8, "ISO-8859-5", "unknown"),
    "serbian.ct":       (8, "ISO-8859-5", "srpski"),
    "ukrainian.ct":     (8, "ISO-8859-5", "unknown"),
    "turkish.ct":       (12, "ISO-8859-9", "türkçe"),
    "russian.ct":       (2104, "windows-1251", "russian")
}

# for colored output
class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

################################################################################

def get_required_version(catalog_path):
    retval = -1
    
    required_version_filename = os.path.join(catalog_path, "catalog_version.h")
    if os.path.exists(required_version_filename):
        required_version_file =  open(required_version_filename, "r")
        required_version_content = required_version_file.read()
        required_version_file.close()

        match = re_ct_reqver.search(required_version_content)
        retval = match.group(1)

    else:
        print(bcolors.WARNING, "Warning! catalog_version.h doesn't exist. No version check!", bcolors.ENDC)

    return int(retval)

################################################################################

check_with_flexcat = True # needs tool flexcat in search path

# read all paths from .gitmodules
module_file = open("../.gitmodules", "r")
module_file_content = module_file.read()
module_path_iter = re_path.finditer(module_file_content)
module_file.close()

report = Report()

# loop through all submodules
for module_path in module_path_iter:
    module_name = module_path.group(1)
    catalog_path = os.path.join("..", module_name)
    print("checking directory", catalog_path)

    required_version = get_required_version(catalog_path)
    
    ct_file_names = glob.glob(catalog_path + "/*.ct")
    
    module = Module(module_name, required_version)
    report.add_module(module)
    
    # loop through all CT files in a directory
    for ct_file_name in ct_file_names:
        # print("checking CT file", ct_file_name)
        # get base of file (which is the language name including .ct)
        ct_file_lang = os.path.basename(ct_file_name)
        
        # blacklist
        if ct_file_lang in ("ahiprefs.ct", "ahi.ct"):
            continue
        
        iana_num, charset, native_language = languages[ct_file_lang]
        
        # check for ## codeset
        # later versions of locale.library might use that to open the right font
        # we must specify the encoding because Python 3 defaults to UTF-8
        ct_file = open(ct_file_name, "r", encoding="ISO-8859-1")
        ct_file_content = ct_file.read()
        ct_file.close()
        match = re_ct_code.search(ct_file_content)
        if match:
            if int(match.group(1)) != iana_num:
                print(bcolors.FAIL, "Codeset error in file", ct_file_name)
                print("IANA num doesn't match! Is: ", match.group(1), "Required:", iana_num, bcolors.ENDC)
                sys.exit(1)
        else:
            print(bcolors.FAIL, "Error in ## codeset of file", ct_file_name, bcolors.ENDC)
            sys.exit(1)
        
        # check ## version
        match = re_ct_ver.search(ct_file_content)
        if match:
            vversion = int(match.group(1))
            if vversion == 0:
                print(bcolors.FAIL, "Version error in file", ct_file_name)
                print("Version number must be > 0! Is: ", match.group(1), bcolors.ENDC)
                sys.exit(1)
        else:
            print(bcolors.FAIL, "Error in ## version of file", ct_file_name, bcolors.ENDC)
            sys.exit(1)
            
        vday = int(match.group(2))
        vmonth = int(match.group(3))
        vyear = int(match.group(4))
        if vday < 1 or vday > 31 or vmonth < 1 or vmonth > 12 \
            or vyear < 1980 or vyear > 2050:
            print(bcolors.FAIL, "Version error in file", ct_file_name)
            print("Invalid values for date! Is: ",
                match.group(2), match.group(3), match.group(4), bcolors.ENDC)
            sys.exit(1)

        # check ## language
        match = re_ct_lang.search(ct_file_content)
        if match:
            if match.group(1) != native_language:
                print(bcolors.FAIL, "Language error in file", ct_file_name)
                print("Language doesn't match! Is: ", match.group(1), "Required:", native_language, bcolors.ENDC)
                sys.exit(1)
        else:
            print(bcolors.FAIL, "Error in ## language of file", ct_file_name, bcolors.ENDC)
            sys.exit(1)


        # check required version
        if required_version !=-1:
            if required_version > vversion:
                print(bcolors.WARNING, "Catalog version is lower than the required version in file", ct_file_name)
                print("Is:", vversion, "Required:", required_version, bcolors.ENDC)
            elif required_version < vversion:
                print(bcolors.FAIL, "Catalog version is higher than the required version in file", ct_file_name)
                print("Is:", vversion, "Required:", required_version, bcolors.ENDC)
            elif check_with_flexcat:
                # if versions match we check correctness with flexcat
                cd_file_names = glob.glob(catalog_path + "/*.cd")
                if len(cd_file_names) == 1:
                    flexcmd = "flexcat {} {} catalog dummy.catalog".format(cd_file_names[0], ct_file_name)
                    res = os.system(flexcmd)
                    if res != 0:
                        print(bcolors.FAIL, "flexcat returned error code", res, "for file", ct_file_name, bcolors.ENDC)
                        sys.exit(1)

        # check encoding
        rawdata = open(ct_file_name, "rb").read()
        result = chardet.detect(rawdata)
        charenc = result['encoding']

        if charenc == "ascii":
            pass
        elif charenc == "utf-8":
            print(bcolors.FAIL, "Encoding error UTF8 in file", ct_file_name, bcolors.ENDC)
            sys.exit(1)
        elif charenc == charset:
            pass
        elif charenc == "ISO-8859-1":
            pass
        else:
            print(bcolors.WARNING, "Encoding mismatch in file", ct_file_name, charenc, bcolors.ENDC)

        module.add_version(ct_file_lang, vversion)


print("-" * 80)

# create the reST file
with open("checkresult.rst", "w") as fh:
    fh.write("=============\n")
    fh.write("Catalog Check\n")
    fh.write("=============\n\n")
    fh.write("This tables are showing a comparison of the catalog version number\n")
    fh.write("a module wants to open and the version of the \*.ct (catalog translation) files.\n\n")
    fh.write("If a catalog's version is highlighted it means:\n\n")
    fh.write("+ n/a (catalog isn't available at all)\n")
    fh.write("+ catalog has a lower version than the required version\n\n")
    
    now = datetime.now(pytz.utc).strftime("%Y-%m-%d %H:%M:%S")
    fh.write("Created on UTC " + now + ".\n\n")

    report.write_rst(fh)
