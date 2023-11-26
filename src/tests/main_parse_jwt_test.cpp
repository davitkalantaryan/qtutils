//
// file:			main_parse_jwt_test.cpp
// path:			src/tests/main_parse_jwt_test.cpp
// created on:		2022 Oct 19
// created by:		Davit Kalantaryan (davit.kalantaryan1980@gmail.com)
//


#include <qtutils/core/parse_jwt.hpp>
#include <qtutils/core/global_functions.hpp>
#include <stdio.h>
#include <string.h>
#include <qtutils/disable_utils_warnings.h>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QJsonArray>

//static const char*const s_cpcSpcTokenDefault = "eyJ0eXAiOiJKV1QiLCJhbGciOiJSUzI1NiJ9.eyJpYXQiOjE2ODIyNTQ1MzAsImV4cCI6MTY4MjI2ODkzMCwicm9sZXMiOlsiUk9MRV9VU0VSIl0sImVtYWlsIjoiaGFydXQxMjI1QGdtYWlsLmNvbSIsInR5cGUiOjF9.wrUMvJdCsm6gpF4PEFE0Bzj-2L_-h2pZ83LN4yAr3HeAg3SKwdnY1eTeUT88WbYq5K79DDIsicPj6EXHFfzf3YysbGycZJikQtViGn1jI6sL3DM_5anEKiwQbWA3J5YHwuiMjMgZwfHjHAivewd2t5xvGqlt9CrAXG5qqPIbGuuKjfsj4ogjuYT9MtEIvC39FIFyXuShKQZP2EY7pZIro2x5lTD3TNiVgC-c2OWa8QN0i07WbSmT1ea-JDzhpCnZEgLRa2WA3eGVI_U6qoSdx_gwZjMQeAGiD3A-smq7UnqHjr0J2BDPGcSubFze708oRcqyDRgwESRI9TpRkcsexg";
  static const char*const s_cpcSpcTokenDefault = "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJlbWFpbCI6ImhhcnV0MTIyNUBnbWFpbC5jb20iLCJleHAiOjE2ODIyNjg5MzAsImlhdCI6MTY4MjI1NDUzMCwicm9sZXMiOlsiUk9MRV9VU0VSIl0sInR5cGUiOjF9.7eqjpPAOcgxbRGkVn5AIeZR8EKZ4PqkA6nuzf-hiR1c";


int main(int a_argc, char* a_argv[])
{
    const char* cpcSpcToken = s_cpcSpcTokenDefault;
    QByteArray baSignature;
    
#if defined(CPPUTILS_DEBUG) && !defined(QTUTILS_SKIPJWT_PARSE_DEBUG)
    if(a_argc>2){
        fprintf(stdout,"Press any key then enter to continue! ");
        fflush(stdout);
        getchar();
    }
#endif
    
    if(a_argc>1){
        cpcSpcToken = a_argv[1];
    }
    
    const QList<QJsonObject> inpList = ::qtutils::core::ParseJWT(cpcSpcToken,&baSignature);
    if(inpList.size()!=2){
        qWarning()<<"Bad JWT token";
        return 1;
    }
    
    qDebug()<<"inpList:  "<<inpList;
    printf("token: %s\n",cpcSpcToken);
    printf("  header\n");
    fflush(stdout);
    QString aString;
    ::qtutils::core::SerializeJsonObject(inpList[0],[&aString](const char* a_cpcStr){
        if(a_cpcStr[0]){
            aString += QString(a_cpcStr);
        }
    },false,"   ");
    puts(aString.toStdString().c_str());
    printf("  payload\n");
    fflush(stdout);
    aString = QString();
    ::qtutils::core::SerializeJsonObject(inpList[1],[&aString](const char* a_cpcStr){
        if(a_cpcStr[0]){
            aString += QString(a_cpcStr);
        }
    },false,"   ");
    puts(aString.toStdString().c_str());
    fflush(stdout);
    printf("  signature\n");
    fflush(stdout);
    printf("    %s\n",baSignature.constData());
    fflush(stdout);
    return 0;
}


#if 0

#include <openssl/evp.h>

int VerifyJwtTest()
{
    unsigned char *sig = NULL;
    EVP_MD_CTX *mdctx = NULL;
	EVP_PKEY_CTX *pkey_ctx = NULL;
	ECDSA_SIG *ec_sig = NULL;
	BIGNUM *ec_sig_r = NULL;
	BIGNUM *ec_sig_s = NULL;
	EVP_PKEY *pkey = NULL;
	const EVP_MD *alg;
	int type;
	int pkey_type;
	BIO *bufkey = NULL;
	int ret = 0;
	int slen;
	size_t padding = 0;

	//switch (jwt->alg) {
	///* RSA */
	//case JWT_ALG_RS256:
		alg = EVP_sha256();
		type = EVP_PKEY_RSA;
	//	break;
	//case JWT_ALG_RS384:
	//	alg = EVP_sha384();
	//	type = EVP_PKEY_RSA;
	//	break;
	//case JWT_ALG_RS512:
	//	alg = EVP_sha512();
	//	type = EVP_PKEY_RSA;
	//	break;

	///* RSA-PSS */
	//case JWT_ALG_PS256:
	//	alg = EVP_sha256();
	//	type = EVP_PKEY_RSA_PSS;
	//	padding = RSA_PKCS1_PSS_PADDING;
	//	break;
	//case JWT_ALG_PS384:
	//	alg = EVP_sha384();
	//	type = EVP_PKEY_RSA_PSS;
	//	padding = RSA_PKCS1_PSS_PADDING;
	//	break;
	//case JWT_ALG_PS512:
	//	alg = EVP_sha512();
	//	type = EVP_PKEY_RSA_PSS;
	//	padding = RSA_PKCS1_PSS_PADDING;
	//	break;

	///* ECC */
	//case JWT_ALG_ES256:
	//	alg = EVP_sha256();
	//	type = EVP_PKEY_EC;
	//	break;
	//case JWT_ALG_ES384:
	//	alg = EVP_sha384();
	//	type = EVP_PKEY_EC;
	//	break;
	//case JWT_ALG_ES512:
	//	alg = EVP_sha512();
	//	type = EVP_PKEY_EC;
	//	break;

	//default:
	//	return EINVAL;
	//}

	sig = jwt_b64_decode(sig_b64, &slen);
	if (sig == NULL)
		VERIFY_ERROR(EINVAL);

	bufkey = BIO_new_mem_buf(jwt->key, jwt->key_len);
	if (bufkey == NULL)
		VERIFY_ERROR(ENOMEM);

	/* This uses OpenSSL's default passphrase callback if needed. The
	 * library caller can override this in many ways, all of which are
	 * outside of the scope of LibJWT and this is documented in jwt.h. */
	pkey = PEM_read_bio_PUBKEY(bufkey, NULL, NULL, NULL);
	if (pkey == NULL)
		VERIFY_ERROR(EINVAL);

	pkey_type = EVP_PKEY_id(pkey);
	if (pkey_type != type)
		VERIFY_ERROR(EINVAL);

	/* Convert EC sigs back to ASN1. */
	if (pkey_type == EVP_PKEY_EC) {
		unsigned int degree, bn_len;
		unsigned char *p;
		EC_KEY *ec_key;

		ec_sig = ECDSA_SIG_new();
		if (ec_sig == NULL)
			VERIFY_ERROR(ENOMEM);

		/* Get the actual ec_key */
		ec_key = EVP_PKEY_get1_EC_KEY(pkey);
		if (ec_key == NULL)
			VERIFY_ERROR(ENOMEM);

		degree = EC_GROUP_get_degree(EC_KEY_get0_group(ec_key));

		EC_KEY_free(ec_key);

		bn_len = (degree + 7) / 8;
		if ((bn_len * 2) != slen)
			VERIFY_ERROR(EINVAL);

		ec_sig_r = BN_bin2bn(sig, bn_len, NULL);
		ec_sig_s = BN_bin2bn(sig + bn_len, bn_len, NULL);
		if (ec_sig_r  == NULL || ec_sig_s == NULL)
			VERIFY_ERROR(EINVAL);

		ECDSA_SIG_set0(ec_sig, ec_sig_r, ec_sig_s);
		jwt_freemem(sig);

		slen = i2d_ECDSA_SIG(ec_sig, NULL);
		sig = jwt_malloc(slen);
		if (sig == NULL)
			VERIFY_ERROR(ENOMEM);

		p = sig;
		slen = i2d_ECDSA_SIG(ec_sig, &p);

		if (slen == 0)
			VERIFY_ERROR(EINVAL);
	}

	mdctx = EVP_MD_CTX_create();
	if (mdctx == NULL)
		VERIFY_ERROR(ENOMEM);

	/* Initialize the DigestVerify operation using alg */
	if (EVP_DigestVerifyInit(mdctx, &pkey_ctx, alg, NULL, pkey) != 1)
		VERIFY_ERROR(EINVAL);

	if (padding > 0 && EVP_PKEY_CTX_set_rsa_padding(pkey_ctx, padding) < 0)
		VERIFY_ERROR(EINVAL);

	/* Call update with the message */
	if (EVP_DigestVerifyUpdate(mdctx, head, head_len) != 1)
		VERIFY_ERROR(EINVAL);

	/* Now check the sig for validity. */
	if (EVP_DigestVerifyFinal(mdctx, sig, slen) != 1)
		VERIFY_ERROR(EINVAL);

jwt_verify_sha_pem_done:
	if (bufkey)
		BIO_free(bufkey);
	if (pkey)
		EVP_PKEY_free(pkey);
	if (mdctx)
		EVP_MD_CTX_destroy(mdctx);
	if (sig)
		jwt_freemem(sig);
	if (ec_sig)
		ECDSA_SIG_free(ec_sig);

	return ret;
}

#endif
