#pragma once
#include <QString>
#include "BaseAccount.h"
#include "BaseAccountType.h"

#include "multimc_logic_export.h"

class BaseAccount;

class MULTIMC_LOGIC_EXPORT BaseProfile: public AuthElement
{
public:
	BaseProfile(BaseAccount * parent)
	{
		m_parent = parent;
	}
	virtual ~BaseProfile() {};
	virtual QString nickname() const = 0;
	virtual QString profileId() const = 0;

	virtual void setNickname(const QString & nickname) = 0;
	virtual void setProfileId(const QString & id) = 0;

	virtual QString typeIcon() const = 0;

	virtual QString typeText() const = 0;

	virtual QString avatar() const
	{
		return QString();
	}

	virtual QString bigAvatar() const
	{
		return avatar();
	}

	virtual void setDefault() override;
	virtual void unsetDefault() override;
	virtual bool isDefault() const override;

	BaseAccount * parent();

	Type getKind() final override
	{
		return Profile;
	}

protected:
	BaseAccount * m_parent = nullptr;
};

Q_DECLARE_METATYPE(BaseProfile *);
