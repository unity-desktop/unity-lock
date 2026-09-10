/* unity-lock-user-page.h
 *
 * Copyright 2026 Muqtadir
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <adwaita.h>

G_BEGIN_DECLS

/**
 * UNITY_LOCK_TYPE_USER_PAGE:
 *
 * The #GType for #UnityLockUserPage.
 */
#define UNITY_LOCK_TYPE_USER_PAGE (unity_lock_user_page_get_type ())

/**
 * UnityLockUserPage:
 *
 * The password prompt, showing the current user and taking their password.
 *
 * The page owns the PAM conversation and emits #UnityLockUserPage::unlocked once
 * the password is accepted.
 */
G_DECLARE_FINAL_TYPE (UnityLockUserPage, unity_lock_user_page,
                      UNITY_LOCK, USER_PAGE, AdwBin)

/**
 * unity_lock_user_page_type_into_entry:
 * @self: a #UnityLockUserPage.
 * @text: characters to append.
 *
 * Appends @text to the password field. This carries over the character that
 * brought the user here, so the first key they press is not swallowed by the
 * page change.
 */
void unity_lock_user_page_type_into_entry (UnityLockUserPage *self,
                                           const gchar       *text);

/**
 * unity_lock_user_page_set_active:
 * @self: a #UnityLockUserPage.
 * @active: %TRUE once the page is the one on screen.
 *
 * Tells the page whether it is the page the user is looking at. An active page
 * takes the keyboard focus. An inactive one clears the field, drops any message,
 * forgets the held secret and gives up the focus. A PAM exchange already in
 * flight keeps running, because AstalAuthPam offers no way to stop one, so an
 * inactive page ignores what it reports.
 */
void unity_lock_user_page_set_active (UnityLockUserPage *self,
                                      gboolean           active);

G_END_DECLS
