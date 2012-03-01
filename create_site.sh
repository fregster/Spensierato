#!/bin/sh

PATH_PWD=`pwd`
echo "Please enter the full path of the Spensierato root directory"
echo "Example: /opt/spensierato or $PATH_PWD"

read PATH_CMS

if [ ! -d $PATH_CMS ]; then
	echo "The path does not exist ($PATH_CMS)"
	exit 1
fi

if [ ! -f $PATH_CMS/spensierato_cms.inc ]; then
	echo "The path provided does not contain Spensierato"
	exit 1
fi

echo "Please enter the full path you wish to install the site to"
echo "Example: /var/www/mynewsite"

read PATH_SITE

if [ ! -d $PATH_SITE ]; then
        echo "The path does not exist ($PATH_SITE)"
        exit 1
fi

echo "Please enter the system user who should own this site"

read USER_SYSTEM

echo "Please enter the system group that needs to access and write to this site (Apache / Web Server)"

read USER_SERVICE

echo "Creating site and linking to Spensierato install"

mkdir $PATH_SITE/writable
mkdir $PATH_SITE/public_html
mkdir $PATH_SITE/config

cp $PATH_CMS/example.htaccess $PATH_SITE/public_html/.htaccess 
cp $PATH_CMS/cms/install/cli/index.php $PATH_SITE/public_html/
echo "require_once('$PATH_CMS/spensierato_cms.inc');" >> $PATH_SITE/public_html/index.php

chown -R $USER_SYSTEM:$USER_SERVICE $PATH_SITE
chmod -R 750 $PATH_SITE
chmod -R 770 $PATH_SITE/writable
chmod -R 770 $PATH_SITE/config

echo "After completing the install the configuration files and dir's should have write permision removed"