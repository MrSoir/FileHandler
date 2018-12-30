#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Dec 28 15:25:46 2018

@author: hippo

dieses skript ermittelt znaechst saemtliche .desktop-files auf dem pc
danach filtert es aus jedem .desktop-file den:
    1. Exec=
    2. Icon=
    3. MimeType=
"""

import os.path
from concurrent.futures import ThreadPoolExecutor
from PIL import Image


def getImageSize(img_pth):
    try:
        img = Image.open(img_pth)
        return img.size
    except:
        return None

def getFileName(path):
    head, tail = os.path.split(path)
    return tail or os.path.basename(head)

def findIconInPathRec(path, icon_name):
    icon_paths = set()
    for e in os.listdir(path):
        abs_path = os.path.join(path, e)
        if os.path.exists(abs_path):
            if os.path.isdir(abs_path):
                ips = findIconInPathRec(abs_path, icon_name)
                for ip in ips:
                    icon_paths.add(ip)
            else:
                icon_img_filename = icon_name + '.'
                if len(e) >= len(icon_img_filename) and e[:len(icon_img_filename)] == icon_img_filename:
                    img_size = getImageSize(abs_path)
                    if img_size:
                        icon_paths.add(abs_path)
    return icon_paths

def findIconPath_hlpr(icon_name, path):
    icon_paths = set()
    if os.path.exists(path):
        ips = findIconInPathRec(path, icon_name)
        for ip in ips:
            icon_paths.add(ip)
    return icon_paths
        
                
def findIconPath(icon_name):
    # icons are located in 1. path1, 2. paths2, 3. path3:
    path1 = '~/.icons'
    paths2 = os.environ['XDG_DATA_DIRS'].split(':')
    path3 = '/usr/share/pixmaps'
    icon_paths = set()
    
    paths = list()
    paths.append(path1)
    for p in paths2:
        paths.append(p)
    paths.append(path3)
    
    executor = ThreadPoolExecutor(max_workers=len(paths))
    futures = list()
    for p in paths:
        f = executor.submit(findIconPath_hlpr, icon_name, p)
        futures.append(f)
    for f in futures:
        ips = f.result()
        for ip in ips:
            icon_paths.add(ip)
    
    # eval the biggest image:
    biggest_img_pth = None
    biggest_img_size = 0
    for icon_pth in icon_paths:
        if os.path.exists(icon_pth):
            size = getImageSize(icon_pth)
            if size:
                w,h = size
                if w * h > biggest_img_size:
                    biggest_img_size = w * h
                    biggest_img_pth = icon_pth
    return biggest_img_pth

resources_path = os.path.join(os.path.dirname(os.path.dirname(__file__)), 'resources')
if not os.path.exists(resources_path):
    try:
        os.mkdir(resources_path)
    except:
        print('could not create dir "' + resources_path + '"')


mimes_resources_path = os.path.join(resources_path, 'mimes')

def getDesktopFilesPaths():
    desktop_file_paths = ('~/.local/share/applications/',
                      '/usr/local/share/applications/',
                      '/usr/share/applications/'
                     )
    return desktop_file_paths

def createMimeResourceDirIfNotExistent():
    if not os.path.exists(mimes_resources_path):
        try:
            os.mkdir(mimes_resources_path)
        except:
            print('could not create dir "' + mimes_resources_path + '"')

            
def writeJSON(json_obj, file_path):
    with open(file_path, 'w') as outfile:
        json.dump(json_obj, outfile, ensure_ascii=False)

def readJSON(file_path):
    with open(file_path, 'r') as infile:
        return json.load(infile)
    
def toGoshStr(json_obj):
    s = ""
    for i,j in enumerate(json_obj):
        s += str(json_obj[j])
        if i < len(json_obj)-1:
            s += '__|__'
    return s

def writeJsonToFileInGoshFormat(json_objects, path):
    with open(path, 'w') as outfile:
        for i, jsn in enumerate(json_objects):
            s = toGoshStr(jsn)
            outfile.write(s)
            if i < len(json_objects)-1:
                outfile.write(os.linesep)

def createMimeJson_hlpr(dsktp_pth):
    json_objects = list()
    if os.path.exists(dsktp_pth):
        for e in [e for e in os.listdir(dsktp_pth) if e[-len('.desktop'):] == '.desktop']:
            json_obj = {'exec': None,
                        'name': None,
                        'icon': None,
                        'icon_path': None,
                        'mimes': None
                        }
            with open(os.path.join(dsktp_pth, e), 'r') as dsktp_out:
                for l in dsktp_out:
                    if l[:len('Exec=')] == 'Exec=':
                        json_obj['exec'] = l.strip()[len('Exec='):]
                    elif l[:len('Name=')] == 'Name=':
                        json_obj['name'] = l.strip()[len('Name='):]
                    elif l[:len('Icon=')] == 'Icon=':
                        icon_name = l.strip()[len('Icon='):]
                        json_obj['icon'] = icon_name
                        icn_pth = findIconPath(icon_name)
                        if icn_pth and os.path.exists(icn_pth):
                            json_obj['icon_path'] = icn_pth
                    elif l[:len('MimeType=')] == 'MimeType=':
                        json_obj['mimes'] = l.strip()[len('MimeType='):]
            json_objects.append(json_obj)
    
    return json_objects
        
def createMimeJson():
    createMimeResourceDirIfNotExistent()
    
    json_path = os.path.join(mimes_resources_path, 'mime_jsons.txt')
    json_objects = list()
    
    dsktp_pths = getDesktopFilesPaths()
    
    executor = ThreadPoolExecutor(max_workers=len(dsktp_pths))
    futures = list()
    
    for dsktp_pth in dsktp_pths:
        f = executor.submit(createMimeJson_hlpr, dsktp_pth)
        futures.append(f)
        
    for f in futures:
        jos = f.result()
        for jo in jos:
            json_objects.append(jo)

    #writeJSON(json_objects, json_path)
    writeJsonToFileInGoshFormat(json_objects, json_path)
    print(str(len(json_objects)) + ' .desktop-files detected')

createMimeJson()
            
            
