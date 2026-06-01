#!/usr/bin/env python3
"""
Fix ScheduleOthersView/ScheduleConstantInspectorView entries and un-vanish all translations.

For each .ts file:
1. Copy the " Value: " (vanished, with leading space) translation to "Value: " (new entry)
2. Mark Schedule Constant/Compact/File entries as finished (they have pre-filled translations)
3. Un-vanish all entries that have non-empty translation text
"""
import re
import glob
import os

def fix_ts_file(filepath):
    with open(filepath, 'rb') as f:
        content = f.read().decode('utf-8')

    original = content

    # Step 1: Find the " Value: " vanished translation and copy it to "Value: " empty entry
    # Extract translation for " Value: " (with leading space, vanished)
    old_value_pattern = re.compile(
        r'<message>\s*'
        r'<source> Value: </source>\s*'
        r'<translation type="vanished">(.*?)</translation>\s*'
        r'</message>',
        re.DOTALL
    )
    m = old_value_pattern.search(content)
    if m:
        value_translation = m.group(1).strip()
        # Apply it to the new "Value: " (without leading space) empty entry
        if value_translation:
            new_value_pattern = re.compile(
                r'(<message>\s*'
                r'<location filename="[^"]*ScheduleConstantInspectorView[^"]*"[^/]*/>\s*'
                r'<source>Value: </source>\s*)'
                r'<translation type="unfinished"></translation>',
                re.DOTALL
            )
            content = new_value_pattern.sub(
                rf'\1<translation>{value_translation}</translation>',
                content
            )

    # Step 2: Mark Schedule Constant/Compact/File as finished (remove type="unfinished" if translation non-empty)
    def mark_finished(m):
        translation_text = m.group(1)
        if translation_text.strip():
            return f'<translation>{translation_text}</translation>'
        return m.group(0)

    content = re.sub(
        r'<translation type="unfinished">([^<]+)</translation>',
        mark_finished,
        content
    )

    # Step 3: Un-vanish all entries with non-empty translation text
    def unvanish(m):
        text = m.group(1)
        if text.strip():
            return f'<translation>{text}</translation>'
        return m.group(0)

    content = re.sub(
        r'<translation type="vanished">(.*?)</translation>',
        unvanish,
        content,
        flags=re.DOTALL
    )

    if content != original:
        with open(filepath, 'wb') as f:
            f.write(content.encode('utf-8'))
        return True
    return False

ts_files = sorted(glob.glob('translations/OpenStudioApp_*.ts'))
print(f'Processing {len(ts_files)} .ts files...')

for ts_file in ts_files:
    lang = os.path.basename(ts_file).replace('OpenStudioApp_', '').replace('.ts', '')
    changed = fix_ts_file(ts_file)
    print(f'  [{lang}] {"updated" if changed else "no change"}')

# Verify final state
print('\nVerification (Spanish):')
with open('translations/OpenStudioApp_es.ts', 'rb') as f:
    content = f.read().decode('utf-8')

total = len(re.findall(r'<message>', content))
vanished = len(re.findall(r'type="vanished"', content))
unfinished = len(re.findall(r'type="unfinished"', content))
print(f'  Total={total}, Vanished={vanished}, Unfinished={unfinished}')

# Check Value: entry
m = re.search(r'<source>Value: </source>\s*<translation[^>]*>(.*?)</translation>', content, re.DOTALL)
if m:
    print(f'  Value: translation = "{m.group(1)}"')
