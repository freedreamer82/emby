#!/bin/bash
# menu.sh - A sample shell script to config emby defines

# Store menu options selected by the user
INPUT=/tmp/menu.sh.$$

# Storage file for displaying cal and date command output
OUTPUT=/tmp/output.sh.$$

INPUT_VALUE=/tmp/input-value.sh.$$

DEBUG=/dev/null
NOT_VALID="deadbeaf"
#DEBUG=/tmp/emby-debug

rm -rf $DEBUG

CONFIG_PATH=./emby_config.h

BACKTITLE="Emby String Config"

VERSION="Sw.Eng Marco Garzola & Emby Team, 1.0"

# get text editor or fall back to vi_editor
vi_editor=${EDITOR-vi}

# trap and delete temp files
trap "rm $OUTPUT; rm $INPUT; exit" SIGHUP SIGINT SIGTERM

declare -A embyconfig

#strings
embyconfig["EMBY_CFG_STRING_LONG_POOL_SIZE"]=10
embyconfig["EMBY_CFG_STRING_SHORT_POOL_SIZE"]=60
embyconfig["EMBY_CFG_STRING_MEDIUM_POOL_SIZE"]=15
#console
embyconfig["EMBY_CFG_CONSOLE_LINE_CURSOR"]=">"
embyconfig["EMBY_CFG_CONSOLE_COMMAND_BUFF_SIZE"]=80
embyconfig["EMBY_CFG_CONSOLE_LOGIN_ROOT_PSW"]="root"
embyconfig["EMBY_CFG_CONSOLE_LOGIN_GUEST_PSW"]="guest"
embyconfig["EMBY_CFG_CONSOLE_USER_LOGIN_TIMEOUT_SEC"]=3
embyconfig["EMBY_CFG_CONSOLE_THREAD_STACK_SIZE"]=500

#
embyconfig["EMBY_CFG_SYSTEM_ERROR_BUFFER_SIZE"]=5

# Log-related defaults (new) - use "true"/"false"
# EMBY_CFG_LOG_IRQ_BUFFER: true = enabled, false = disabled
embyconfig["EMBY_CFG_LOG_IRQ_BUFFER"]="false"
# EMBY_CFG_DISABLE_LOGS: false = logs enabled, true = logs disabled
embyconfig["EMBY_CFG_DISABLE_LOGS"]="false"
# EMBY_CFG_LOG_IRQ_BUFFER_SIZE: numeric size for IRQ log buffer
embyconfig["EMBY_CFG_LOG_IRQ_BUFFER_SIZE"]=5

# List of boolean keys that should be omitted from generated header when set to false
SKIP_IF_FALSE=("EMBY_CFG_DISABLE_LOGS" "EMBY_CFG_LOG_IRQ_BUFFER")

#hashmap["key"]="value"
#hashmap["key2"]="value2"
#echo "${hashmap["key"]}"
#for key in ${!hashmap[@]}; do echo $key; done
#for value in ${hashmap[@]}; do echo $value; done
#echo hashmap has ${#hashmap[@]} elements

function save_config_file() {

  if [ -f $CONFIG_PATH ]; then
    rm $CONFIG_PATH
  fi
  dialog --clear --nocancel --backtitle "$BACKTITLE" --title "Config Path" --inputbox "" 0 0 "$CONFIG_PATH" 2>CONFIG_PATH

  #order the key first
  keys=$(echo ${!embyconfig[@]} | tr ' ' $'\n' | sort)
  for k in ${keys[@]}; do

    # Skip boolean keys that are set to false
    skip=0
    for b in "${SKIP_IF_FALSE[@]}"; do
      if [ "$k" == "$b" ] && [ "${embyconfig[$k]}" == "false" ]; then
        skip=1
        break
      fi
    done
    if [ $skip -eq 1 ]; then
      continue
    fi

    VALUE="${embyconfig["$k"]}"

    # If the value is the literal 'true' or 'false', handle specially
    if [[ "$VALUE" == "true" ]]; then
      # emit a bare define without a value (so #ifdef KEY works)
      printf "#define  %s\n" $k >>$CONFIG_PATH
      continue
    elif [[ "$VALUE" == "false" ]]; then
      # shouldn't happen here because SKIP_IF_FALSE prevents false booleans in the list,
      # but keep the guard: skip
      continue
    elif [[ "$VALUE" == ?(-)+([0-9]) ]]; then
      # is a number... fallthrough to formatted print
      :
    elif [ $VALUE == $NOT_VALID ]; then
      VALUE=
      echo $VALUE >>$DEBUG
    else
      VALUE="\"${VALUE}\""
    fi

    printf "#define  %-50s  %-10s\n" $k $VALUE >>$CONFIG_PATH

  done
}

function change_value_popup() {
  KEY="$1"
  dialog --clear --nocancel --backtitle "$BACKTITLE" --title "Value" --inputbox "" 0 0 "${embyconfig[$KEY]}" 2>$INPUT_VALUE

  VALUE=$(cat $INPUT_VALUE)
  #  echo $VALUE >>$DEBUG
  embyconfig[$KEY]=$VALUE
  #  echo "${embyconfig[$KEY]}" >>$DEBUG
}

# New helper to change boolean values using a small menu
function change_boolean_popup() {
  KEY="$1"
  CURRENT="${embyconfig[$KEY]}"
  dialog --backtitle "$BACKTITLE" \
    --title "Toggle" \
    --clear \
    --menu "Select value for $KEY" 10 50 2 \
    true "Enable" \
    false "Disable" 2>"${INPUT}"

  sel=$(<"${INPUT}")
  if [ -n "$sel" ]; then
    embyconfig[$KEY]=$sel
  fi
}

function show_container() {
  dialog --backtitle "$BACKTITLE" \
    --title "[ EMBY CONTAINERS ]" \
    --clear \
    --no-tags \
    --separate-output \
    --checklist "Select items:" 0 0 0 \
    1 "Disable Circular Buffer Critical Section[default enable]" Off \
    2 "Disable Buffer Critical Section[default enable]" Off \
    2>"$INPUT"

  menuitem=$(<"${INPUT}")
  OPTIONS=$(cat $INPUT | tr -s ' ' '\n')

  for field in ${OPTIONS}; do
    if [ $field == 1 ]; then
      embyconfig["EMBY_CFG_DISABLE_CIRCULAR_BUFFER_CRITICAL_SECTIONS"]=$NOT_VALID
    elif [ $field == 2 ]; then
      embyconfig["EMBY_CFG_DISABLE_BUFFER_CRITICAL_SECTIONS"]=$NOT_VALID
    fi
  done
}

function show_console() {

  dialog --backtitle "$BACKTITLE" \
    --title "[ EMBY CONSOLE ]" \
    --clear \
    --menu "" 15 50 4 \
    "Line Cursor" "${embyconfig["EMBY_CFG_CONSOLE_LINE_CURSOR"]}" \
    "Commands Buffer Size" "${embyconfig["EMBY_CFG_CONSOLE_COMMAND_BUFF_SIZE"]}" \
    "Root Password" "${embyconfig["EMBY_CFG_CONSOLE_LOGIN_ROOT_PSW"]}" \
    "Guest Password" "${embyconfig["EMBY_CFG_CONSOLE_LOGIN_GUEST_PSW"]}" \
    "Login Timeout Seconds" "${embyconfig["EMBY_CFG_CONSOLE_USER_LOGIN_TIMEOUT_SEC"]}" \
     "Console thread stack size" "${embyconfig["EMBY_CFG_CONSOLE_THREAD_STACK_SIZE"]}" \
    2>"${INPUT}"

  menuitem=$(<"${INPUT}")

  case $menuitem in
  "Line Cursor") change_value_popup EMBY_CFG_CONSOLE_LINE_CURSOR ;;
  "Root Password") change_value_popup EMBY_CFG_CONSOLE_COMMAND_BUFF_SIZE ;;
  "Commands Buffer Size") change_value_popup EMBY_CFG_CONSOLE_LOGIN_ROOT_PSW ;;
  "Guest Password") change_value_popup EMBY_CFG_CONSOLE_LOGIN_GUEST_PSW ;;
  "Login Timeout Seconds") change_value_popup EMBY_CFG_CONSOLE_USER_LOGIN_TIMEOUT_SEC ;;
  *)
    echo "???"
    ;;
  esac
}

function show_system() {

  dialog --backtitle "$BACKTITLE" \
    --title "[ EMBY SYSTEM ]" \
    --clear \
    --menu "" 15 50 4 \
    "Error Pool Size" "${embyconfig["EMBY_CFG_SYSTEM_ERROR_BUFFER_SIZE"]}" 2>"${INPUT}"

  menuitem=$(<"${INPUT}")

  case $menuitem in
  "Error Pool Size") change_value_popup EMBY_CFG_SYSTEM_ERROR_BUFFER_SIZE ;;
  *)
    echo "???"
    ;;
  esac

}
function show_strings() {

  dialog --backtitle "$BACKTITLE" \
    --title "[ EMBY STRINGS ]" \
    --clear \
    --menu "" 15 50 4 \
    "Pool Size Long Strings" "${embyconfig["EMBY_CFG_STRING_LONG_POOL_SIZE"]}" \
    "Pool Size Medium String" "${embyconfig["EMBY_CFG_STRING_MEDIUM_POOL_SIZE"]}" \
    "Pool Size Short Strings" "${embyconfig["EMBY_CFG_STRING_SHORT_POOL_SIZE"]}" 2>"${INPUT}"

  menuitem=$(<"${INPUT}")

  case $menuitem in
  "Pool Size Long Strings") change_value_popup EMBY_CFG_STRING_LONG_POOL_SIZE ;;
  "Pool Size Medium Strings") change_value_popup EMBY_CFG_STRING_MEDIUM_POOL_SIZE ;;
  "Pool Size Short Strings") change_value_popup EMBY_CFG_STRING_SHORT_POOL_SIZE ;;
  *)
    echo "???"
    ;;
  esac

}

# --- New: Logs section ---
function show_logs() {
  dialog --backtitle "$BACKTITLE" \
    --title "[ EMBY LOGS ]" \
    --clear \
    --menu "" 15 60 4 \
    "Enable IRQ Buffer" "${embyconfig["EMBY_CFG_LOG_IRQ_BUFFER"]}" \
    "Disable Logs" "${embyconfig["EMBY_CFG_DISABLE_LOGS"]}" \
    "IRQ Buffer Size" "${embyconfig["EMBY_CFG_LOG_IRQ_BUFFER_SIZE"]}" 2>"${INPUT}"

  menuitem=$(<"${INPUT}")

  case $menuitem in
  "Enable IRQ Buffer") change_boolean_popup EMBY_CFG_LOG_IRQ_BUFFER ;;
  "Disable Logs") change_boolean_popup EMBY_CFG_DISABLE_LOGS ;;
  "IRQ Buffer Size") change_value_popup EMBY_CFG_LOG_IRQ_BUFFER_SIZE ;;
  *)
    echo "???"
    ;;
  esac
}

#
# set infinite loop
#
while true; do

  ### display main menu ###
  dialog --backtitle "Emby Config" \
    --title "[ EMBY CONFIGURATOR ]" \
    --clear \
    --nocancel \
    --menu "You can use the UP/DOWN arrow keys, the first \n\
letter of the choice as a hot key, or the \n\
number keys 1-9 to choose an option.\n\
Choose the TASK" 18 70 4 \
    Strings "Strings config" \
    System "System config" \
    Console "Console config" \
    Containers "Containers config" \
    Logs "Logs config" \
    "Save Config" "Export Configuration in a file" \
    Exit "Exit to the shell" 2>"${INPUT}"

  menuitem=$(<"${INPUT}")

  # make decsion
  case $menuitem in
  Strings) show_strings ;;
  System) show_system ;;
  Console) show_console ;;
  Containers) show_container ;;
  Logs) show_logs ;;
  "Save Config") save_config_file ;;
  Exit)
    exit 0
    break
    ;;
  esac

done

# if temp files found, delete em
[ -f $OUTPUT ] && rm $OUTPUT
[ -f $INPUT ] && rm $INPUT
