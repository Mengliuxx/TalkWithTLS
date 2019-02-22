#!/usr/bin/python3

import pytest

from common import *

log_filename = log_dir + '/' + os.path.basename(__file__) + '.txt'

global log_fd;

def TWT_LOG(log_filename, str):
    fd = open(log_filename, 'a')
    fd.write(str)
    fd.close()

def log_std_out_and_err(exe, out, err):
    TWT_LOG(log_filename, exe + '\n')
    TWT_LOG(log_filename, '-----------------------------------------------\n')
    if out != None:
        TWT_LOG(log_filename, str(out) + '\n')
    if err != None:
        TWT_LOG(log_filename, '-----------------------------------------------\n')
        TWT_LOG(log_filename, str(err) + '\n')

def log_procs(proc1, proc2):
    (out, err) = proc1.communicate()
    log_std_out_and_err('Server', out, err)
    (out, err) = proc2.communicate()
    log_std_out_and_err('Client', out, err)

def log_tc(apps):
    TWT_LOG(log_filename, '===============================================\n')
    TWT_LOG(log_filename, 'Running ' + apps[0]  + ' vs ' + apps[1] + ' ...\n')
    TWT_LOG(log_filename, '===============================================\n')

def validate_tc_result(ret1, ret2):
    if ret1 != 0 or ret2 != 0:
        TWT_LOG(log_filename, '###FAILED !!!!\n\n')
        result = -1
    else:
        TWT_LOG(log_filename, '###Succeeded\n\n')
        result = 0
    return result

def validate_app(app):
    if not os.path.isfile(bin_dir + '/' + app):
        TWT_LOG(log_filename, 'Test Apps [' + bin_dir + '/' + app + '] not found !!!\n')
        return -1
    return 0

def validate_apps(apps):
    log_tc(apps)
    if len(apps) != 2:
        TWT_LOG('Count [' + str(len(apps)) + 'of Apps passed is invalid !!!\n');
    assert validate_app(apps[0]) == 0
    assert validate_app(apps[1]) == 0

def run_serv_clnt_app(apps):
    validate_apps(apps)
    proc1 = subprocess.Popen([bin_dir + "/" + apps[0]], stdout=subprocess.PIPE)
    proc2 = subprocess.Popen([bin_dir + "/" + apps[1]], stdout=subprocess.PIPE)
    ret1 = proc1.wait()
    ret2 = proc2.wait()
    log_procs(proc1, proc2)
    return validate_tc_result(ret1, ret2)

# Test Sample Code
t12_testcases = [
                    ['openssl_tls12_server', 'openssl_tls12_client'],
                    ['openssl_tls12_verify_cb_server', 'openssl_tls12_verify_cb_client'],
                ]

def test_tls12_sample_code():
    for apps in t12_testcases:
        assert run_serv_clnt_app(apps) == 0

t13_testcases = [
                    ['openssl_tls13_server', 'openssl_tls13_client'],
                    #['openssl_tls13_dhe_server', 'openssl_tls13_dhe_client'],
                    ['wolfssl_tls13_server', 'wolfssl_tls13_client']
                ]

def test_tls13_sample_code():
    for apps in t13_testcases:
        assert run_serv_clnt_app(apps) == 0